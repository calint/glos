#!/bin/bash
# tools:
#   clang-tidy: Ubuntu LLVM version 15.0.7
set -e
cd $(dirname "$0")
DIR="$(pwd)"

SRC="../../src/main.cpp ../../src/application/application.cpp"

date | tee clang-tidy.log
clang-tidy --config-file=clang-tidy.cfg -header-filter=.* $SRC -- -std=c++20 | tee -a clang-tidy.log
date | tee -a clang-tidy.log

