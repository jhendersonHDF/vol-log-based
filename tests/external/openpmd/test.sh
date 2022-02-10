#!/bin/bash
#
# Copyright (C) 2022, Northwestern University and Argonne National Laboratory
# See COPYRIGHT notice in top-level directory.
#

# Exit immediately if a command exits with a non-zero status.
set -e

source ${top_builddir}/utils/h5lenv.bash
export LD_LIBRARY_PATH="./build/lib":${LD_LIBRARY_PATH}

if test "x$#" = x0 ; then
    RUN=""
else
    RUN="${TESTMPIRUN}"
fi

echo "${RUN} ./8a_benchmark_write_parallel > 8a_benchmark_write_parallel.log"
${RUN} ./8a_benchmark_write_parallel > 8a_benchmark_write_parallel.log

# echo "${RUN} ./8b_benchmark_read_parallel ../samples/8a_parallel_3Db_0000001.h5 sy > 8b_benchmark_read_parallel.log"
# ${RUN} ./8b_benchmark_read_parallel ../samples/8a_parallel_3Db_0000001.h5 sy > 8b_benchmark_read_parallel.log