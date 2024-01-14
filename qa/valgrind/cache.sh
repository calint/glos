#!/bin/bash
valgrind --tool=cachegrind --cache-sim=yes ./glos
