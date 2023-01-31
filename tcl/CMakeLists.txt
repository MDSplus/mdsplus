
###
### tcl_commands
###

file(GLOB
    _source_list
    "*.c"
)

add_library(
    tcl_commands
    ${_source_list}
)

install(TARGETS tcl_commands)

target_link_libraries(
    tcl_commands
    PUBLIC
        MdsServerShr
)

###
### Scripts
###

if(WIN32)
    set(_ext ".bat")
endif()

install(
    FILES mdstcl${_ext}
    PERMISSIONS ${MODE_755}
    TYPE BIN
)