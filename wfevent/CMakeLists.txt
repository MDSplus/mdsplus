
###
### wfevent
###

FILE(GLOB
    _source_list
    "*.c"
)

ADD_EXECUTABLE(
    wfevent
    ${_source_list}
)

INSTALL(TARGETS wfevent)

TARGET_LINK_LIBRARIES(
    wfevent
    PUBLIC
        TdiShr
)
