
macro(set_environment_for_tests ) # ARGN=...

    set(_env_mods
        "PYTHONPATH=set:${CMAKE_SOURCE_DIR}/python"
        "MDS_PYDEVICE_PATH=set:${CMAKE_SOURCE_DIR}/pydevices"
        "MDS_PATH=set:${CMAKE_SOURCE_DIR}/tdi"
    )

    # TODO: default_tree_path ?

    # TODO: ports ?

    if(WIN32)
        list(APPEND _env_mods "PATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    else()
        list(APPEND _env_mods "LD_LIBRARY_PATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    endif()

    set_tests_properties(
        ${ARGN}
        PROPERTIES
            ENVIRONMENT_MODIFICATION "${_env_mods}"
    )

endmacro()