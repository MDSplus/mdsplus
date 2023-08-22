
###
### MdsServerShr
###

file(GLOB
    _source_list
    "*.c"
)

add_library(
    MdsServerShr
    ${_source_list}
)

target_link_libraries(
    MdsServerShr
    PUBLIC
        Mdsdcl
)

mdsplus_add_static_copy(MdsServerShr _static_target)

install(TARGETS MdsServerShr ${_static_target})