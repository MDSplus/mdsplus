spawn killall mdsip
spawn killall actlog
set command tcl
env test_path=/tmp
env ACTION_SERVER=localhost:7777
env ACTION_MONITOR=localhost:7778
spawn echo -e "multi | $USER\n*">mdsip.hosts
spawn/nowait mdsip -s -p 7777 -h mdsip.hosts >dispatch_server.log  2>&1
spawn/nowait mdsip -s -p 7778 -h mdsip.hosts >dispatch_monitor.log 2>&1
spawn/nowait actlog -monitor ACTION_MONITOR  >dispatch_logger.log  2>&1
wait 3
set tree test
create pulse 1
set tree test/shot=1
dispatch/nowait D1:ACTIONSERVER:MANUAL
show server ACTION_SERVER
dispatch/nowait D2:ACTIONSERVER:MANUAL
show server ACTION_SERVER
dispatch D3:ACTIONSERVER:MANUAL
show server ACTION_SERVER
dispatch/build/monitor=ACTION_MONITOR
dispatch/phase/monitor=ACTION_MONITOR init
wait 1
show server ACTION_SERVER
dispatch/phase/monitor=ACTION_MONITOR store
wait 1
show server ACTION_SERVER
dispatch/close
close/all
stop server ACTION_SERVER
stop server ACTION_MONITOR
wait 1
spawn killall actlog
