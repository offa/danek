#!/bin/bash

set -ex

## Install GTest
GTEST_VERSION=master
GTEST=googletest-${GTEST_VERSION}
OlD_DIR=$PWD

wget https://github.com/google/googletest/archive/${GTEST_VERSION}.tar.gz
tar -xzf *.tar.gz
cd ${GTEST}
mkdir build && cd build

if [[ "${CXX}" = clang* ]] ; then
    export CXXFLAGS="-stdlib=libc++"
fi

cmake ..
make -j 4
sudo make install
cd ${OLD_DIR}

