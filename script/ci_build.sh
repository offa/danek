#!/bin/bash

set -ex


if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"
fi


mkdir build && cd build

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_DEMOS=ON -DBUILD_SCHEMA_TESTS=ON ..
make
make unittest

