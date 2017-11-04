#!/bin/bash

set -e

./clean.sh
cd raw/glove
tar -czf ../../glove.tar.gz dataset.dat queries.dat answers.dat
cd ../..
cd raw/mnist
tar -czf ../../mnist.tar.gz dataset.dat queries.dat answers.dat
cd ../..
cd raw/sift
tar -czf ../../sift.tar.gz dataset.dat queries.dat answers.dat
cd ../..
cd raw/sift_subsample
tar -czf ../../sift_subsample.tar.gz dataset.dat queries.dat answers.dat
cd ../..
