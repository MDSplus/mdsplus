help set verify
set verify
set verify
set command tcl
help
help tree commands
help edit commands
help dispatch commands
help abort server
type before comment
!abort server gub
type after comment
abort server localhost:9000
!abort server localhost:9001
env tcltest_path=/tmp
help edit
edit/new tcltest
help add node
add node gub
add node numeric/usage=numeric
!add node foo/usage=gub
add node .child
help add tag
add tag gub bar
help dir
dir
dir/full
dir foo/full
help directory/tag
dir/tag
dir/tag/path
dir/tag bar
help close
!close
close/confirm
edit/new tcltest
add node gub/usage=numeric
help write
write
close
set tree tcltest
help put
put gub 42
help decompile
decompile gub
help show data
show data gub
put/extend gub
42*100

decompile gub
show data gub
close
help clean
clean tcltest/shot=-1
help compress
compress tcltest/shot=-1
set tree tcltest
help create pulse
create pulse 1
set tree tcltest/shot=1
close/all
set tree tcltest
help delete pulse
delete pulse 1
delete pulse -1
close
help set current
set current tcltest 42
help show current
show current tcltest

