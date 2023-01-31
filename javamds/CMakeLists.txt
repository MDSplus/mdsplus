
if(ENABLE_JAVA)

    ###
    ### JavaMds
    ###

    add_library(
        JavaMds SHARED
        ServerSupport.c
        mdsobjects.c
    )

    install(TARGETS JavaMds)

    target_link_libraries(
        JavaMds
        PUBLIC
            MdsIpShr
            JNI::JNI
    )

endif()