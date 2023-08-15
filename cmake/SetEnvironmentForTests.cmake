
# TODO: Make it clear that _env_mods can be used to add extra environment variables
macro(set_environment_for_tests ) # ARGN=...

    list(APPEND _env_mods
        "MDSPLUS_DIR=set:${CMAKE_SOURCE_DIR}"
        "MDS_PATH=set:${CMAKE_SOURCE_DIR}/tdi"
        "MDS_PYDEVICE_PATH=set:${CMAKE_SOURCE_DIR}/pydevices"
        "JAVA_PATH_NAME=set:${Java_JAVA_EXECUTABLE} -Djava.library.path=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
        "PYTHON=set:${Python_EXECUTABLE}"
        "PyLib=set:${Python_LIBRARIES}"
        "PYTHONPATH=set:${CMAKE_SOURCE_DIR}/python"
        "PATH=path_list_prepend:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        "PATH=path_list_prepend:${CMAKE_SOURCE_DIR}/mdstcpip" # Needed for mdsip-client-* and mdsip-server-*
    )

    # TODO: default_tree_path ?

    # TODO: ports ?

    if(WIN32)
        list(APPEND _env_mods "PATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    else()
        list(APPEND _env_mods "LD_LIBRARY_PATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

        if(APPLE)
            list(APPEND _env_mods "DYLD_LIBRARY_PATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
        endif()
    endif()

    set_tests_properties(
        ${ARGN}
        PROPERTIES
            ENVIRONMENT_MODIFICATION "${_env_mods}"
    )

endmacro()