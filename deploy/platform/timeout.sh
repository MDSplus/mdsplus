#!/bin/bash
shift $(1)
(
    sleep $1
    kill -s SIGINT $$
    sleep 10
    kill -s SIGTERM $$ && kill -0 $$ || exit 0
    sleep 5
    kill -s SIGKILL $$
) 2> /dev/null &
shift
:&& exec "$@"
exit $?
