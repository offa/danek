#!/bin/bash

set -ex

mkdir build && cd build

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_DEMOS=ON -DBUILD_SCHEMA_TESTS=ON ..
make
make unittest

