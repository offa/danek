#!/bin/bash

set -ex

if [[ "${CXX}" == clang* ]]
then
    sudo ln -s ${DEPENDENCY_DIR}/llvm-source/projects/libcxxabi/include/cxxabi.h /usr/include/cxxabi.h
    sudo ln -s ${DEPENDENCY_DIR}/llvm-source/projects/libcxxabi/include/__cxxabi_config.h /usr/include/__cxxabi_config.h
fi


