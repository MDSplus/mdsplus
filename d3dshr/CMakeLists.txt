
IF(ENABLE_D3D)

    ###
    ### MdsD3D
    ###

    ADD_LIBRARY(
        MdsD3D SHARED
        mdsptdata.f
    )

    INSTALL(TARGETS MdsD3D)

    TARGET_LINK_LIBRARIES(
        MdsD3D
        PUBLIC
            ${LIBD3}
    )

ENDIF()