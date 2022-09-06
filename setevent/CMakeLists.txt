
###
### setevent
###

FILE(GLOB
    _source_list
    "*.c"
)

ADD_EXECUTABLE(
    setevent
    ${_source_list}
)

INSTALL(TARGETS setevent)

TARGET_LINK_LIBRARIES(
    setevent
    PUBLIC
        MdsShr
)
