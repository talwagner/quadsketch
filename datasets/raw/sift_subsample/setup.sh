#!/bin/bash

set -e

./clean.sh
make
wget ftp://ftp.irisa.fr/local/texmex/corpus/sift.tar.gz
tar -xf sift.tar.gz
./step1
./step2
