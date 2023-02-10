
IF(ENABLE_D3D)

    ###
    ### MdsD3D
    ###

    add_library(
        MdsD3D
        mdsptdata.f
    )

    install(TARGETS MdsD3D)

    target_link_libraries(
        MdsD3D
        PUBLIC
            ${D3}
    )

ENDIF()