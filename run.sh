#! /bin/bash

./build.sh

cd build || { echo "Failed to enter build directory"; exit 1; }

./main "$@"

cd ..