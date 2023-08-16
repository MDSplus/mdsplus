
# Call add_test() and configure the MDSplus environment variables.
#
# mdsplus_add_test(NAME <name> COMMAND <command> [<arg>...])
#                  [WORKING_DIRECTORY <dir>]
#                  [ENVIRONMENT_MODIFICATIONS <mods>])
#
# https://cmake.org/cmake/help/latest/command/add_test.html
# https://cmake.org/cmake/help/latest/prop_test/ENVIRONMENT_MODIFICATION.html
#
macro(mdsplus_add_test)

    # Boolean options
    set(_boolean_options)
    # Regular Arguments
    set(_single_value_options NAME WORKING_DIRECTORY)
    # Variable Arguments
    set(_multi_value_options COMMAND ENVIRONMENT_MODIFICATION)

    cmake_parse_arguments(
        _add_test
        "${_boolean_options}"
        "${_single_value_options}"
        "${_multi_value_options}"
        ${ARGN}
    )

    if(NOT DEFINED _add_test_WORKING_DIRECTORY)
        set(_add_test_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endif()

    file(RELATIVE_PATH _add_test_path ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    string(REPLACE "\\" "/" _add_test_path "${_add_test_path}")

    set(_add_test_target "${_add_test_path}/${_add_test_NAME}")
    
    add_test(
        NAME ${_add_test_target}
        COMMAND ${_add_test_COMMAND}
        WORKING_DIRECTORY ${_add_test_WORKING_DIRECTORY}
    )
    
    list(APPEND _add_test_env_mods
        # Used to find the rest of MDSplus
        "MDSPLUS_DIR=set:${CMAKE_SOURCE_DIR}"

        # Used to search for TDI and Python scripts, and to add TDI drivers
        "MDS_PATH=set:${CMAKE_CURRENT_SOURCE_DIR}"
        "MDS_PATH=cmake_list_append:${CMAKE_SOURCE_DIR}/tdi"

        # Used to run or load Python
        "PYTHON=set:${Python_EXECUTABLE}"
        "PyLib=set:${Python_LIBRARIES}"

        # Needed for `import MDSplus` in Python
        "PYTHONPATH=set:${CMAKE_SOURCE_DIR}/python"

        # Needed to add Python drivers 
        "MDS_PYDEVICE_PATH=set:${CMAKE_SOURCE_DIR}/pydevices"

        # Needed to run MDSplus executables
        "PATH=path_list_prepend:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"

        # Several tests make use of the default trees
        "main_path=set:${CMAKE_SOURCE_DIR}/trees"
        "subtree_path=set:${CMAKE_SOURCE_DIR}/trees/subtree"

        # Needed for mdsip-client-* and mdsip-server-*
        "PATH=path_list_prepend:${CMAKE_SOURCE_DIR}/mdstcpip"
    )

    if(WIN32)
        # Windows searches $PATH for loading .dll's
        list(APPEND _add_test_env_mods
            "PATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
        )
    else()
        # Linux searches $LD_LIBRARY_PATH for loading .so's
        # This is set for Apple as well for backwards compatibility
        list(APPEND _add_test_env_mods
            "LD_LIBRARY_PATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
        )

        if(APPLE)
            # Apple searches $DYLD_LIBRARY_PATH for loading .dylib's
            list(APPEND _add_test_env_mods
                "DYLD_LIBRARY_PATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
            )
        endif()
    endif()

    list(APPEND _add_test_env_mods ${_add_test_ENVIRONMENT_MODIFICATION})
    
    set_tests_properties(
        ${_add_test_target}
        PROPERTIES
            ENVIRONMENT_MODIFICATION "${_add_test_env_mods}"
            FAIL_REGULAR_EXPRESSION "FAILED"
    )

endmacro()