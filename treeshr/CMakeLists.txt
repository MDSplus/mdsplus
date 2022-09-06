
FILE(GLOB
    _source_list
    "*.c"
)

ADD_LIBRARY(
    TreeShr SHARED
    ${_source_list}
)

INSTALL(TARGETS TreeShr)

TARGET_LINK_LIBRARIES(
    TreeShr
    PUBLIC
        MdsShr
)

ADD_STATIC_COPY(TreeShr)