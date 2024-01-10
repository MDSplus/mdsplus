
if(TARGET JNI::JNI)

    ###
    ### JavaMds
    ###

    add_library(
        JavaMds
        ServerSupport.c
        mdsobjects.c
    )

    target_link_libraries(
        JavaMds
        PUBLIC
            MdsIpShr
            JNI::JNI
    )

    install(TARGETS JavaMds)

endif()