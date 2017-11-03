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


    echo "*** Searching for cxxabi.h ***"
    find /usr/ -type f -name "cxxabi.h"
    echo "****"
    ls /usr/local/clang-3.9.0/include/c++/v1
    echo "++++++++++++++"
    find ${DEPENDENCY_DIR} -type f -name "cxxabi.h"

    sudo ln -s ${DEPENDENCY_DIR}/llvm-source/projects/libcxxabi/include/cxxabi.h /usr/include/cxxabi.h
    ls ${DEPENDENCY_DIR}/llvm-source/projects/libcxxabi/include/
    sudo ln -s ${DEPENDENCY_DIR}/llvm-source/projects/libcxxabi/include/__cxxabi_config.h /usr/include/__cxxabi_config.h
    ls -l /usr/include/

fi

mkdir -p build-${CC} && cd build-${CC}

cmake ${BUILD_FLAGS} ..
make -j4
sudo make install


cd ${BUILD_DIR}

