
###
### setevent
###

add_executable(
    setevent
    setevent.c
)

install(TARGETS setevent)

target_link_libraries(
    setevent
    PUBLIC
        MdsShr
)
