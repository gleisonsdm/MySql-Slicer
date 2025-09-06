#!/bin/bash

git clone https://github.com/mysql/mysql-connector-cpp.git

cd mysql-connector-cpp
mkdir build
cd build

cmake ..
make
make install
