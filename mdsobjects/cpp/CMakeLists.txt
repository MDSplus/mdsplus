
###
### MdsObjectsCppShr
###

add_library(
    MdsObjectsCppShr
    mdsdataobjects.cpp
    mdstreeobjects.cpp
    mdsipobjects.cpp
    mdseventobjects.cpp
    mdsdatastreams.cpp
    mdsdata.c
    mdstree.c
)

install(TARGETS MdsObjectsCppShr)

target_link_libraries(
    MdsObjectsCppShr
    PUBLIC
        MdsIpShr
)

add_static_copy(MdsObjectsCppShr)