
###
### JavaMds
###

ADD_LIBRARY(
    JavaMds SHARED
    ServerSupport.c
    mdsobjects.c
)

INSTALL(TARGETS JavaMds)

TARGET_LINK_LIBRARIES(
    JavaMds
    PUBLIC
        MdsIpShr
        JNI::JNI
)