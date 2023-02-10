
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

    target_link_libraries(
        ${_roam_target}
        PUBLIC
            TdiShr
            CURL::libcurl
            Globus::Globus
    )

    add_static_copy(${_roam_target})

    install(TARGETS ${_roam_target} ${_roam_target}-static)

    ###
    ### Roam
    ###

    set(_generic_target "${CMAKE_SHARED_LIBRARY_PREFIX}Roam${CMAKE_SHARED_LIBRARY_SUFFIX}")

    add_custom_command(
        TARGET ${_roam_target} POST_BUILD
        BYPRODUCTS ${_generic_target}
        COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE_NAME:${_roam_target}> ${_generic_target}
        WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    )

    install(
        FILES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${_generic_target}
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