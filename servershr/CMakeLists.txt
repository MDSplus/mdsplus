
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

install(TARGETS MdsServerShr)

target_link_libraries(
    MdsServerShr
    PUBLIC
        Mdsdcl
)

add_static_copy(MdsServerShr)