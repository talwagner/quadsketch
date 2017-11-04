#include "serialize.h"

#include <chrono>
#include <iostream>
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

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

using ir::serialize;

const size_t MAX_LENGTH = 100000;

char buf[MAX_LENGTH];

int main() {
    try{
        FILE *input = fopen("glove.twitter.27B.100d.txt", "r");
        size_t counter = 0;
        vector<vector<float>> dataset;
        while (fgets(buf, MAX_LENGTH, input)) {
            stringstream ss(buf);
            string word;
            ss >> word;
            float coord;
            vector<float> current_row;
            while (ss >> coord) {
                current_row.push_back(coord);
            }
            dataset.push_back(current_row);
            ++counter;
            if (counter % 10000 == 0) {
                cout << counter << " rows processed" << endl;
                //break;
            }
        }
        fclose(input);
        cout << "serializing" << endl;
        auto t1 = high_resolution_clock::now();
        serialize("dataset_unnormalized.dat", dataset);
        auto t2 = high_resolution_clock::now();
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
