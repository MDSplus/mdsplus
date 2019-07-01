help set verify
set verify
set verify
set stoponfail/off
set command tcl
help
help tree commands
help edit commands
help dispatch commands
help abort server
type before comment
abort server gub
type after comment
abort server localhost:9000
abort server localhost:9001
env tcltest_path=/tmp
env tcltestsub_path=/tmp
help edit
edit/new tcltest
help add node
add node gub
add node numeric/usage=numeric
add node foo/usage=gub
rename foo foobar
rename foobar gub
rename foobar foo
add node foo/usage=signal
add node .child
help add tag
add tag gub bar
put gub foo
delete node foo
show data gub
help remove tag
remove tag bar
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
close
close/confirm
edit/new tcltest
add node gub/usage=numeric
add node bar/usage=numeric
add node .tcltestsub
set node .tcltestsub/subtree
help write
write
write tcltest/shot=-1
close
edit/new tcltestsub
add node node1/usage=signal
add node node2/usage=action
add node node3/usage=task
!add node node4/model=a12
add node node5/usage=text
add node node6/usage=numeric
add node .child1
add node .child1:numeric/usage=numeric
add node .child1:numeric2/usage=numeric
help delete
delete node .child1:numeric
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

show data gub
put/extend gub
42+foo(42)

show data gub
put/extend gub
foobar+42

show data gub
put/extend gub
bar+42+size("testing")

show data gub
put/extend gub
_gub

show data gub
put/extend gub
.tcltestsub.child1:numeric+1

show data gub
put/extend gub
[1,2,3,4,5,6,7]

show data gub
put/extend gub
set_range(-1 : 8,[1,2,3,4,5,6,7,8,9,10])

show data gub
put/ext gub
[[1,2],[3,4],[5,6]]

show data gub
decompile gub
show data gub
help set node
set node gub/compress_on_put
set node gub/nocompress_on_put
set node gub/off
set node gub/on
set node gub/write_once
set node gub/nowrite_once
set node gub/shot_write
set node gub/noshot_write
set node gub/model_write
set node gub/nomodel_write
put/extend .tcltestsub:node1
build_signal(1 : 100, *, 1 : 100)

show data .tcltestsub:node1
put/extend .tcltestsub:node2
Build_Action(Build_Dispatch(2, "CAMAC_SERVER", "INIT", *, *), Build_Method(*, "INIT", *), *, *, *)

show data .tcltestsub:node2
show data .tcltestsub:node4
help set attribute
help show attribute
set attribute gub/name=special 42
show attribute gub/name=special
set attribute gub/name=special2/extended
build_with_units(42,"volts")

show attribut gub/name=special2
show attribute gub
close
help clean
clean tcltest/shot=-1
set tree "tcltest,gub"
close
help compress
compress tcltest/shot=-1
set tree tcltest
help set default
set default tcltestsub.child1
set default \tcltest::top
set default sadasd
show def
help create pulse
create pulse 1
set node tcltestsub/noinclude
create pulse/conditional 2
set tree tcltest/shot=1
verify
help show db
show db
help show versions
show versions
close/all
set tree tcltest
help delete pulse
set tree tcltest/shot=1
close
delete pulse 1
delete pulse 2
delete pulse -1
close
close/all
help set current
set current tcltest 42
help show current
show current tcltest
help setevent
setevent testtcl
help wfevent
type waiting for testtcl event
wfevent testtcl/timeout=1
close/all
verify
create pulse 3
write
set default gub
show default

