#!/bin/sh
set -e

cd templates/ && sh make_src.sh && cd .. && ./build.sh
