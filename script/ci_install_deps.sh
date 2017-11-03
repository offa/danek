#!/bin/bash

set -ex


BUILD_DIR=${TRAVIS_BUILD_DIR}

mkdir -p "${DEPENDENCY_DIR}" && cd "${DEPENDENCY_DIR}"


#--- GTest
if [[ ! -d "${DEPENDENCY_DIR}/googletest" ]]
then
    git clone --depth=1 -b master https://github.com/google/googletest googletest
fi

cd googletest

if [[ "${CXX}" == clang* ]]
then
    BUILD_FLAGS="${BUILD_FLAGS} -DCMAKE_CXX_FLAGS=-stdlib=libc++"
fi

mkdir -p build-${CC} && cd build-${CC}

cmake ${BUILD_FLAGS} ..
make -j4
sudo make install


cd ${BUILD_DIR}

