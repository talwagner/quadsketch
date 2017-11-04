#!/bin/bash

set -e

./clean.sh
make
wget http://nlp.stanford.edu/data/glove.twitter.27B.zip
unzip glove.twitter.27B.zip
./step1
./step2
