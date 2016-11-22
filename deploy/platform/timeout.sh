#!/bin/bash
(
    sleep ${1}s
    kill -s SIGINT $$ && kill -s 0 $$ || exit 0
    sleep 10s
    kill -s SIGTERM $$ && kill -s 0 $$ || exit 0
    sleep 5s
    kill -s SIGKILL $$ || exit 0
    exit 1
) 2> /dev/null &
shift
:&& exec "$@"
exit $?
