
# TODO: Make it clear that _env_mods can be used to add extra environment variables
macro(set_environment_for_tests ) # ARGN=...

    list(APPEND _env_mods
        "MDSPLUS_DIR=set:${CMAKE_SOURCE_DIR}"
        "MDS_PATH=set:${CMAKE_SOURCE_DIR}/tdi"
        "MDS_PYDEVICE_PATH=set:${CMAKE_SOURCE_DIR}/pydevices"
        "JAVA_PATH_NAME=set:${Java_JAVA_EXECUTABLE}"
        "PYTHON=set:${Python_EXECUTABLE}"
        "PYTHONPATH=set:${CMAKE_SOURCE_DIR}/python"
    )

    # TODO: default_tree_path ?

    # TODO: ports ?

    if(WIN32)
        list(APPEND _env_mods "PATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    elseif(APPLE)
        list(APPEND _env_mods "DYLD_LIBRARY_PATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    else()
        list(APPEND _env_mods "LD_LIBRARY_PATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    endif()

    set_tests_properties(
        ${ARGN}
        PROPERTIES
            ENVIRONMENT_MODIFICATION "${_env_mods}"
    )

endmacro()