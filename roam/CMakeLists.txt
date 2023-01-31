
if(Globus_FOUND AND Globus_FLAVOR)

    ###
    ### Roam_${Globus_FLAVOR}
    ###

    set(_roam_target "Roam_${Globus_FLAVOR}")

    add_library(
        ${_roam_target}
        roam_check_access.c
        roam_gridmap_callout.c
    )

    install(TARGETS ${_roam_target})

    target_link_libraries(
        ${_roam_target}
        PUBLIC
            TdiShr
            Globus::Globus
            ${LIBCURL}
    )

    add_static_copy(${_roam_target})

    ###
    ### Roam
    ###

    set(_generic_target "${CMAKE_SHARED_LIBRARY_PREFIX}Roam${CMAKE_SHARED_LIBRARY_SUFFIX}")

    add_custom_command(
        TARGET ${_roam_target} POST_BUILD
        BYPRODUCTS ${_generic_target}
        COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE_NAME:${_roam_target}> ${_generic_target}
        WORKING_DIRECTORY ${LIB_DIR}
    )

    install(
        FILES ${LIB_DIR}/${_generic_target}
        TYPE LIB
    )

    ###
    ### Scripts
    ###

    install(
        FILES roam_check_access.sh RENAME roam_check_access
        TYPE BIN
    )

endif()