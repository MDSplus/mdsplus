
###
### XTreeShr
###

FILE(GLOB
    _source_list
    "*.c"
)

ADD_LIBRARY(
    XTreeShr SHARED
    ${_source_list}
)

INSTALL(TARGETS XTreeShr)

TARGET_LINK_LIBRARIES(
    XTreeShr
    PUBLIC
        TdiShr
)

ADD_STATIC_COPY(XTreeShr)