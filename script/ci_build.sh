#!/bin/bash

set -ex


# Conan
apt-get install -y python3-pip
pip3 install -U conan
conan profile new default --detect

if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"
    conan profile update settings.compiler.libcxx=libc++ default
else
    conan profile update settings.compiler.libcxx=libstdc++11 default
fi


mkdir build && cd build

conan install --build=missing ..


export GTEST_BRIEF=1

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_DEMOS=ON -DBUILD_SCHEMA_TESTS=ON ..
make
make unittest
