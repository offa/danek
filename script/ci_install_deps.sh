#!/bin/bash

set -ex

if [[ "${CXX}" = clang* ]] ; then
    export CXXFLAGS="-stdlib=libc++"
fi


BUILD_DIR=${TRAVIS_BUILD_DIR}

mkdir -p "${DEPENDENCY_DIR}" && cd "${DEPENDENCY_DIR}"


#--- GTest
if [[ ! -d "${DEPENDENCY_DIR}/googletest" ]]
then
    git clone --depth=1 -b master https://github.com/google/googletest googletest
fi


cd googletest
mkdir -p build && cd build

cmake ..
make -j4
sudo make install


cd ${BUILD_DIR}

