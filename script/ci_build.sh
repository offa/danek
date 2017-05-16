#!/bin/bash

set -ex

mkdir build && cd build

cmake -DCMAKE_BUILD_TYPE=Release -DDEMOS=ON -DSCHEMA_TESTS=ON ..
make
make unittest

