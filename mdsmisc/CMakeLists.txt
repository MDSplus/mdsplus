
###
### MdsMisc
###

FILE(GLOB
    _source_list
    "*.c"
)

ADD_LIBRARY(
    MdsMisc SHARED
    ${_source_list}
)

INSTALL(TARGETS MdsMisc)

TARGET_LINK_LIBRARIES(
    MdsMisc
    PUBLIC
        XTreeShr
)

ADD_STATIC_COPY(MdsMisc)
