
IF(Globus_FOUND AND Globus_FLAVOR)

    ###
    ### Roam_${Globus_FLAVOR}
    ###

    SET(_roam_target "Roam_${Globus_FLAVOR}")

    ADD_LIBRARY(
        ${_roam_target} SHARED
        roam_check_access.c
        roam_gridmap_callout.c
    )

    INSTALL(TARGETS ${_roam_target})

    TARGET_LINK_LIBRARIES(
        ${_roam_target}
        PUBLIC
            TdiShr
            Globus::Globus
            ${LIBCURL}
    )

    ADD_STATIC_COPY(${_roam_target})

    ###
    ### Roam
    ###

    SET(_generic_target "${CMAKE_SHARED_LIBRARY_PREFIX}Roam${CMAKE_SHARED_LIBRARY_SUFFIX}")

    ADD_CUSTOM_COMMAND(
        TARGET ${_roam_target} POST_BUILD
        BYPRODUCTS ${_generic_target}
        COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE_NAME:${_roam_target}> ${_generic_target}
        WORKING_DIRECTORY ${LIB_DIR}
    )

    INSTALL(
        FILES ${LIB_DIR}/${_generic_target}
        TYPE LIB
    )

    ###
    ### roam_check_access
    ###

    INSTALL(
        FILES roam_check_access.sh RENAME roam_check_access
        TYPE BIN
    )

ENDIF()