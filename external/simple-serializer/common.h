#ifndef _COMMON_H_
#define _COMMON_H_

#include <Eigen/Dense>

#include <vector>

namespace ir {

using std::vector;

typedef Eigen::VectorXf Point;

void normalize(vector<Point> *dataset) {
    for (auto &x: *dataset) {
        x.normalize();
    }
}

};

#endif
