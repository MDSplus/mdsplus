edit distest /shot=1 /new
add node act01 /usage=action
write
put/extend ACT01
Build_Action(Build_Dispatch(2,"ACTION_SERVER","INIT",10,*),BUILD_FUNCTION(BUILTIN_OPCODE("COMMA"),BUILD_FUNCTION(BUILTIN_OPCODE("WRITE"),*,"Test action"),1))

close
