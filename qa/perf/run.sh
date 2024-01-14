#!/bin/bash
# enable prof for users
#  :: echo -1 | sudo tee /proc/sys/kernel/perf_event_paranoid
# -1
#  :: cat /proc/sys/kernel/perf_event_paranoid
# -1

perf record -B -e cache-references,cache-misses,branches,branch-misses,cycles,instructions,faults,migrations ./glos 
perf report
