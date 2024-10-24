#! /bin/bash


cd build || { echo "Failed to enter build directory"; exit 1; }

./main "$@"

cd ..