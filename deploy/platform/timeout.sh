#!/bin/bash
(
    waitandkill() {
        # waitandkill seconds pid SIGNAL exitcode
        sleep ${1}s
        if ( -z kill -s 0 $2 )
        then
            echo WARNING: Sending $3 after $1
            kill -s SIGINT $2
        else
            exit $4
        fi
    }
    waitandkill $1 $$ SIGINT  0
    waitandkill 10 $$ SIGTERM 1
    waitandkill  5 $$ SIGKILL 2
) 2> /dev/null &
shift
:&& exec "$@"
exit $?
