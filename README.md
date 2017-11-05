# QuadSketch

Implementation of the QuadSketch algorithm from the paper:
Piotr Indyk, Ilya Razenshteyn, Tal Wagner, "Practical Data-Dependent Metric Compression with Provable Guarantees", NIPS 2017

Instructions:
1. Run /datasets/download.sh to download the datasets
2. In src, compile and run qs.cpp. For example:
qs -i ../datasets/sift -o sift_results.txt -n 8 -d 7 -l 5
