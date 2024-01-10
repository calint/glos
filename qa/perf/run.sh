#!/bin/bash
perf record -B -e cache-references,cache-misses,branches,branch-misses,cycles,instructions,faults,migrations ./glos 
perf report
