#!/bin/bash
(
    waitandkill() {
        # waitandkill seconds pid SIGNAL exitcode
        # send signal to pid after seconds
        # or exits with exitcode
        sleep ${1}s
        if ( -z kill -s 0 $2 )
        then
            echo WARNING: Sending $3 after $1
            kill -s $3 $2
        else
            exit $4
        fi
    }
    # kill wiith sigabrt to trigger core dump
    waitandkill $1 $$ SIGABRT 3
    ## try to interrupt main program at first
    #waitandkill $1 $$ SIGINT  0
    ## try to gracefully terminate main program if sigint did not help
    #waitandkill 10 $$ SIGTERM 1
    ## kill main program if sigterm did not help
    #waitandkill  5 $$ SIGKILL 2
) 2> /dev/null &
# store pid of watchdog fork
watchdog=$!
# strip timeout.sh  from args
shift
# execute main program in same pid
:&& exec "$@"
# store exitcode
status=$?
# terminate watchdog
kill -s SIGTERM $watchdog
exit $status
