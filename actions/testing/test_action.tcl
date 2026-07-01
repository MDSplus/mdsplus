dispatch /command /server=DISPATCH_SERVER set tree distest /shot=1
wait 1
dispatch /command /server=DISPATCH_SERVER dispatch/build/monitor=MONITOR_SERVER
wait 1
dispatch /command /server=DISPATCH_SERVER dispatch/phase/monitor=MONITOR_SERVER init
wait 1
