
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

target_link_libraries(
    tcl_commands
    PUBLIC
        MdsServerShr
)

install(TARGETS tcl_commands)

###
### Scripts
###

if(WIN32)
    set(_ext ".bat")
endif()

set(_script_list
    mdstcl${_ext}
)

file(COPY ${_script_list}
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

install(
    FILES ${_script_list}
    PERMISSIONS ${MODE_755}
    TYPE BIN
)