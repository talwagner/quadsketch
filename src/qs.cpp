#include "serialize.h"

#include <boost/program_options.hpp>

#include <chrono>
#include <fstream>
#include <map>
#include <iostream>
#include <random>
#include <vector>

using namespace std;
using namespace std::chrono;
using namespace ir;
namespace po = boost::program_options;

vector<Point> new_dataset;

void solve(const vector<Point> &dataset,
           int b1,
           int b2,
           const vector<pair<float, float>> &bb,
           const vector<int> &who,
           int depth,
           int max_depth,
           uint64_t *num_leaves,
           uint64_t *dfs_size,
	   uint64_t *reduced_dfs_size,
	   uint64_t *num_reduced_edges,
	   int *max_reduced_edge,
           const Point &cur,
	   int long_edge_length,
	   int lambda) {
    int n = dataset.size();
    int d = b2 - b1;
    int dd = d / 8;
    if (d % 8) ++dd;
    if (long_edge_length - lambda >= *max_reduced_edge) {
      *max_reduced_edge = long_edge_length - lambda;
    }
    if (depth >= max_depth) {
        ++(*num_leaves);
        for (auto x: who) {
            for (int i = 0; i < d; ++i) {
                new_dataset[x][b1 + i] = cur[i];
            }
        }
        return;
    }
    vector<float> mm;
    for (auto x: bb) {
        mm.push_back((x.first + x.second) / 2.0);
    }
    map<vector<uint8_t>, vector<int>> parts;
    for (auto x: who) {
        vector<uint8_t> code(dd, 0);
        for (int j = 0; j < d; ++j) {
            if (dataset[x][b1 + j] < mm[j]) {
            }
            else {
                code[j / 8] |= 1 << (j % 8);
            }
        }
        parts[code].push_back(x);
    }
    for (auto x: parts) {
        vector<pair<float, float>> newbb(bb.size());
        Point new_p(cur);
        for (int j = 0; j < d; ++j) {
            newbb[j] = bb[j];
            if ((x.first[j / 8] >> (j % 8)) & 1) {
                newbb[j].first = mm[j];
		if (long_edge_length >= lambda && parts.size() == 1) {
		  continue;
		}
		new_p[j] += 1.0 / (2.0 * (1 << depth));
            }
            else {
                newbb[j].second = mm[j];
            }
        }
        (*dfs_size) += 2 + d;
	(*reduced_dfs_size) += 2;
	if (long_edge_length >= lambda && parts.size() == 1) {
	}
	else {
	  (*reduced_dfs_size) += 1 + d;
	}
	if (long_edge_length == lambda && parts.size() == 1) {
	  ++(*num_reduced_edges);
	  (*reduced_dfs_size) += 1;
	}
        solve(dataset,
	      b1,
	      b2,
	      newbb,
	      x.second,
	      depth + 1,
	      max_depth,
	      num_leaves,
	      dfs_size,
	      reduced_dfs_size,
	      num_reduced_edges,
	      max_reduced_edge,
	      new_p,
	      (parts.size() > 1) ? 1 : long_edge_length + 1,
	      lambda);
    }
}

int main(int argc, char **argv) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "print usage message")
        ("input,i", po::value<string>(), "input dataset")
        ("output,o", po::value<string>(), "output file")
        ("depth,d", po::value<int>(), "depth of a tree")
        ("num_blocks,n", po::value<int>(), "number of blocks")
        ("lambda,l", po::value<int>(), "compression parameter")
        ("num_queries,q", po::value<int>(), "number of queries used for evaluation")
        ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        cout << desc << endl;
        return 0;
    }
    if (!vm.count("input") || !vm.count("output") || !vm.count("depth") || !vm.count("num_blocks") || !vm.count("lambda")) {
      cout << desc << endl;
      throw runtime_error("input dataset, output file, number of blocks, the compression parameter and depth of a tree must be specified");
    }
    string input_folder = vm["input"].as<string>();
    string output_file = vm["output"].as<string>();
    int num_blocks = vm["num_blocks"].as<int>();
    int depth = vm["depth"].as<int>();
    int lambda = vm["lambda"].as<int>();
    if (num_blocks <= 0) {
      cout << desc << endl;
      throw runtime_error("number of blocks must be positive");
    }
    if (depth <= 0) {
      cout << desc << endl;
      throw runtime_error("depth must be positive");
    }
    if (lambda <= 0) {
      cout << desc << endl;
      throw runtime_error("lambda must be positive");
    }
    if (lambda > depth) {
      cout << desc << endl;
      throw runtime_error("lambda must be at most depth");
    }
    random_device rd;
    mt19937_64 gen(rd());
    vector<Point> dataset;
    vector<Point> queries;
    vector<vector<uint32_t>> answers;
    deserialize(input_folder + "/dataset.dat", &dataset);
    deserialize(input_folder + "/queries.dat", &queries);
    deserialize(input_folder + "/answers.dat", &answers);
    if (vm.count("num_queries")) {
      int num_queries = vm["num_queries"].as<int>();
      if (num_queries > queries.size()) {
	cout << desc << endl;
	throw runtime_error("too many queries");
      }
      queries.erase(queries.begin() + num_queries, queries.end());
    }
    int q = queries.size();
    for (auto x: queries) {
        dataset.push_back(x);
    }
    int n = dataset.size();
    int d = dataset[0].size();
    cout << n << " " << d << endl;
    float cmin = 1e100;
    float cmax = -1e100;
    for (int i = 0; i < d; ++i) {
        for (int j = 0; j < n; ++j) {
            cmin = min(cmin, dataset[j][i]);
            cmax = max(cmax, dataset[j][i]);
        }
    }
    float delta = cmax - cmin;
    cmin -= delta;
    uniform_real_distribution<float> u(0.0, delta);
    vector<int> all(n);
    for (int i = 0; i < n; ++i) {
        all[i] = i;
    }
    int start = 0;
    uint64_t total = 0;
    new_dataset.resize(n);
    for (int i = 0; i < n; ++i) {
        new_dataset[i].resize(d);
    }
    for (int block_id = 0; block_id < num_blocks; ++block_id) {
        int block_len = d / num_blocks;
        if (block_id < d % num_blocks) {
            ++block_len;
        }
        cout << start << " " << start + block_len << endl;
        vector<pair<float, float>> bb;
        for (int i = 0; i < d; ++i) {
            float s = u(gen);
            bb.push_back(make_pair(cmin + s, cmax + s));
        }
        uint64_t num_leaves = 0;
        uint64_t dfs_size = 0;
        uint64_t reduced_dfs_size = 0;
	uint64_t num_reduced_edges = 0;
	int max_reduced_edge = 0;
        solve(dataset, start, start + block_len, bb, all, 0, depth, &num_leaves, &dfs_size, &reduced_dfs_size, &num_reduced_edges, &max_reduced_edge, Point::Zero(block_len), 0, lambda);
	cout << num_leaves << " " << reduced_dfs_size << " " << num_reduced_edges << " " << max_reduced_edge << endl;
        start += block_len;
        total += reduced_dfs_size;
        int t = 0;
        while (1ll << t < num_leaves) {
            ++t;
        }
        total += n * t;
        t = 0;
        while (1ll << t < max_reduced_edge) {
            ++t;
        }
	total += num_reduced_edges * t;
    }
    cout << total << endl;
    int counter = 0;
    double distortion = 0.0;
    for (int i = 0; i < q; ++i) {
        float best_score = 1e100;
        int who = -1;
        for (int j = 0; j < n - q; ++j) {
            float score = (new_dataset[j] - new_dataset[n - q + i]).squaredNorm();
            if (score < best_score) {
                best_score = score;
                who = j;
            }
        }
	float dd = (dataset[answers[i][0]] - queries[i]).norm();
	if (dd < 1e-3) {
	  distortion += 1.0;
	  ++counter;
	  cout << "+" << flush;
	}
	else {
	  distortion += (dataset[who] - queries[i]).norm() / (dataset[answers[i][0]] - queries[i]).norm();
	  if (who == answers[i][0]) {
            ++counter;
	    cout << "+" << flush;
	  }
	  else {
	    cout << "-" << flush;
	  }
	}
    }
    cout << endl;
    cout << (counter + 0.0) / (q + 0.0) << endl;
    cout << "distortion " << (distortion + 0.0) / (q + 0.0) << endl;
    ofstream output(output_file);
    output << "method qs" << endl;
    output << "dataset " << input_folder << endl;
    output << "num_blocks " << num_blocks << endl;
    output << "depth " << depth << endl;
    output << "lambda " << lambda << endl;
    output << "size " << total << endl;
    output << "nn_accuracy " << (counter + 0.0) / (q + 0.0) << endl;
    output << "distortion " << (distortion + 0.0) / (q + 0.0) << endl;
    output.close();
    return 0;
}
