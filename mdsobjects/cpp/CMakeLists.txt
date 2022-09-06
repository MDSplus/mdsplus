
###
### MdsObjectsCppShr
###

ADD_LIBRARY(
    MdsObjectsCppShr SHARED
    mdsdataobjects.cpp
    mdstreeobjects.cpp
    mdsipobjects.cpp
    mdseventobjects.cpp
    mdsdatastreams.cpp
    mdsdata.c
    mdstree.c
)

INSTALL(TARGETS MdsObjectsCppShr)

TARGET_LINK_LIBRARIES(
    MdsObjectsCppShr
    PUBLIC
        MdsIpShr
)

ADD_STATIC_COPY(MdsObjectsCppShr)