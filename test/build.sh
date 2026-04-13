#!/bin/bash

cd build/ || exit 1
cmake --build . && ./jsonfish_test
cd ../..
