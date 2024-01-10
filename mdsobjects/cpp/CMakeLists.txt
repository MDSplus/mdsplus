
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

target_link_libraries(
    MdsObjectsCppShr
    PUBLIC
        MdsIpShr
)

mdsplus_add_static_copy(MdsObjectsCppShr _static_target)

install(TARGETS MdsObjectsCppShr ${_static_target})