#!/bin/bash

set -e

./clean.sh
wget https://s3.amazonaws.com/sketches-datasets/glove.tar.gz
wget https://s3.amazonaws.com/sketches-datasets/mnist.tar.gz
wget https://s3.amazonaws.com/sketches-datasets/sift.tar.gz
wget https://s3.amazonaws.com/sketches-datasets/sift_subsample.tar.gz
mkdir -p glove
mkdir -p mnist
mkdir -p sift
mkdir -p sift_subsample
tar -xzf glove.tar.gz -C glove
tar -xzf mnist.tar.gz -C mnist
tar -xzf sift.tar.gz -C sift
tar -xzf sift_subsample.tar.gz -C sift_subsample
