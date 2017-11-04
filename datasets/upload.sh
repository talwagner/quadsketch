#!/bin/bash

set -e

aws s3 cp glove.tar.gz s3://sketches-datasets/
aws s3 cp mnist.tar.gz s3://sketches-datasets/
aws s3 cp sift.tar.gz s3://sketches-datasets/
aws s3 cp sift_subsample.tar.gz s3://sketches-datasets/

