
###
### tditest
###

FILE(GLOB
    _source_list
    "*.c"
)

ADD_EXECUTABLE(
    tditest
    ${_source_list}
)

INSTALL(TARGETS tditest)

TARGET_LINK_LIBRARIES(
    tditest
    PUBLIC
        TdiShr
        Readline::Readline
)
