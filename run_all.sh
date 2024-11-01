#! /bin/bash

./build.sh

# self joins
./run.sh -R T3WKT -S T3WKT -p 1000 -d PARAGRAPHS -o t3t9t10_v2.csv
./run.sh -R T9WKT -S T9WKT -p 1000 -a -d PARAGRAPHS -o t3t9t10_v2.csv
./run.sh -R T10WKT -S T10WKT -p 10000 -a -d PARAGRAPHS -o t3t9t10_v2.csv

# rest of the joins
./run.sh -R T3WKT -S T9WKT -p 10000 -a -d PARAGRAPHS -o t3t9t10_v2.csv
./run.sh -R T3WKT -S T10WKT -p 1000 -a -d PARAGRAPHS -o t3t9t10_v2.csv
./run.sh -R T9WKT -S T10WKT -p 10000 -a -d PARAGRAPHS -o t3t9t10_v2.csv

