
if(TARGET Globus::Globus AND TARGET CURL::libcurl)

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

    mdsplus_add_static_copy(${_roam_target} _static_target)

    install(TARGETS ${_roam_target} ${_static_target})

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
    
    set(_script_list
        roam_check_access
    )

    file(COPY ${_script_list}
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

    install(
        FILES ${_script_list}
        TYPE BIN
        PERMISSIONS ${MODE_755}
    )

endif()