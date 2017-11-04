#!/bin/bash

set -e

./clean.sh
make
wget http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz
wget http://yann.lecun.com/exdb/mnist/t10k-images-idx3-ubyte.gz
gunzip train-images-idx3-ubyte.gz
gunzip t10k-images-idx3-ubyte.gz
python parse.py
./step2
