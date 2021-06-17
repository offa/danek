#!/bin/bash

set -ex


if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"
fi

export GTEST_BRIEF=1

mkdir build && cd build

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_DEMOS=ON -DBUILD_SCHEMA_TESTS=ON ..
make
make unittest
