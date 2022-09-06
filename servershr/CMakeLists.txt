
###
### MdsServerShr
###

FILE(GLOB
    _source_list
    "Server*.c"
)

ADD_LIBRARY(
    MdsServerShr SHARED
    ${_source_list}
)

INSTALL(TARGETS MdsServerShr)

TARGET_LINK_LIBRARIES(
    MdsServerShr
    PUBLIC
        Mdsdcl
)

ADD_STATIC_COPY(MdsServerShr)