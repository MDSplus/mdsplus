set command tcl
env test_path=/tmp
edit test/new
add node D1/model=TestDevice
put D1:ACTIONSERVER "'ACTION_SERVER'"
add node D2/model=TestDevice
put D2:ACTIONSERVER "'ACTION_SERVER'"
add node D3/model=TestDevice
put D3:ACTIONSERVER "'ACTION_SERVER'"
write
