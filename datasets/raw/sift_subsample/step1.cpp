#include "serialize.h"

#include <chrono>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <cstdio>

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::runtime_error;
using std::stringstream;
using std::string;
using std::vector;
using std::mt19937_64;
using std::uniform_int_distribution;

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

using ir::serialize;

const size_t NUM_QUERIES = 10000;
const size_t SEED = 4057218;

int main() {
    try{
        FILE *input = fopen("sift/sift_base.fvecs", "rb");
        size_t counter = 0;
        vector<vector<float>> dataset;
        while (1) {
            int d;
            if (!fread(&d, sizeof(int), 1, input)) {
                break;
            }
            vector<float> current_row(d);
            fread(&current_row[0], sizeof(float), d, input);
            dataset.push_back(current_row);
            ++counter;
            if (counter % 10000 == 0) {
                cout << counter << " rows processed" << endl;
                //break;
            }
        }
        fclose(input);
	mt19937_64 gen(SEED);
	vector<vector<float>> queries;
	for (int i = 0; i < NUM_QUERIES; ++i) {
	  uniform_int_distribution<> u(0, dataset.size() - 1);
	  int k = u(gen);
	  queries.push_back(dataset[k]);
	  dataset.erase(dataset.begin() + k);
	}
        cout << "serializing" << endl;
        auto t1 = high_resolution_clock::now();
        serialize("dataset.dat", dataset);
        auto t2 = high_resolution_clock::now();
        cout << "done" << endl;
        cout << duration_cast<duration<double>>(t2 - t1).count() << endl;
        cout << "serializing" << endl;
        t1 = high_resolution_clock::now();
        serialize("queries.dat", queries);
        t2 = high_resolution_clock::now();
        cout << "done" << endl;
        cout << duration_cast<duration<double>>(t2 - t1).count() << endl;
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
