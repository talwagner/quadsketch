#include "common.h"
#include "serialize.h"

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::greater;
using std::make_pair;
using std::mt19937_64;
using std::nth_element;
using std::pair;
using std::runtime_error;
using std::sort;
using std::uniform_int_distribution;
using std::vector;

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

using ir::deserialize;
using ir::normalize;
using ir::Point;
using ir::serialize;

const size_t K = 10;

void generate_answers(const vector<Point> &dataset,
                      const vector<Point> &queries,
                      vector<vector<uint32_t>> *answers) {
    vector<pair<float, uint32_t>> scores(dataset.size());
    answers->resize(queries.size());
    size_t outer_counter = 0;
    for (const auto &query: queries) {
        size_t inner_counter = 0;
        for (const auto &data_point: dataset) {
            float score = (query - data_point).squaredNorm();
            scores[inner_counter] = make_pair(score, inner_counter);
            ++inner_counter;
        }
        nth_element(scores.begin(), scores.begin() + K - 1, scores.end());
        sort(scores.begin(), scores.begin() + K);
        (*answers)[outer_counter].resize(K);
        for (size_t i = 0; i < K; ++i) {
            (*answers)[outer_counter][i] = scores[i].second;
            //cout << dataset[scores[i].second].dot(query) / query.norm() / dataset[scores[i].second].norm() << " ";
        }
        //cout << endl;
        ++outer_counter;
        if (outer_counter % 100 == 0) {
            cout << outer_counter << "/" << queries.size() << endl;
        }
    }
}

int main() {
    try {
        vector<Point> dataset;
        cout << "reading dataset" << endl;
        deserialize("dataset.dat", &dataset);
        cout << "done" << endl;
        cout << dataset.size() << " points read" << endl;

        vector<Point> queries;

        cout << "reading queries" << endl;
        deserialize("queries.dat", &queries);
        cout << "done" << endl;
        cout << queries.size() << " points read" << endl;

        vector<vector<uint32_t>> answers;

        cout << "generating answers" << endl;
        auto t1 = high_resolution_clock::now();
        generate_answers(dataset, queries, &answers);
        auto t2 = high_resolution_clock::now();
        cout << "done" << endl;
        cout << duration_cast<duration<double>>(t2 - t1).count() / queries.size() << " seconds per query" << endl;

        cout << "writing answers" << endl;
        serialize("answers.dat", answers);
        cout << "done" << endl;
    }
    catch (runtime_error &e) {
        cerr << "runtime error: " << e.what() << endl;
        return 1;
    }
    catch (exception &e) {
        cerr << "exception: " << e.what() << endl;
        return 1;
    }
    catch (...) {
        cerr << "Unknown error" << endl;
        return 1;
    }
    return 0;
}
