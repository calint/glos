#!/bin/bash
sudo perf record -B -e cache-references,cache-misses,branches,branch-misses,cycles,instructions,faults,migrations ./glos 
sudo perf report
