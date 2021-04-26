#!/bin/sh
valgrind --quiet --trace-children=yes --error-exitcode=1 \
    --suppressions=valgrind-stdlib.supp \
    --suppressions=valgrind-dlerr.supp \
    --suppressions=valgrind-connection.supp \
    --suppressions=valgrind-treenode.supp \
    --dsymutil=yes --leak-check=yes \
    --gen-suppressions=all \
    "$@" \
    2>&1 | $(dirname $0)/parse_valgrind_suppressions.sh
