
###
### TdiShr
###

FILE(GLOB
    _source_list
    "*.c"
)

ADD_LIBRARY(
    TdiShr SHARED
    ${_source_list}
)

INSTALL(TARGETS TdiShr)

TARGET_LINK_LIBRARIES(
    TdiShr
    PUBLIC
        TreeShr
)

ADD_STATIC_COPY(TdiShr)