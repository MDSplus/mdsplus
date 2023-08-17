
# Call add_test() and configure the MDSplus environment variables.
#
# mdsplus_add_test(NAME <name> COMMAND <command> [<arg>...])
#                  [WORKING_DIRECTORY <dir>]
#                  [ENVIRONMENT_MODIFICATIONS <mods>]
#                  [NO_VALGRIND])
#
# https://cmake.org/cmake/help/latest/command/add_test.html
# https://cmake.org/cmake/help/latest/prop_test/ENVIRONMENT_MODIFICATION.html
#
macro(mdsplus_add_test)

    # Boolean options
    set(_boolean_options NO_VALGRIND)
    # Regular Arguments
    set(_single_value_options NAME WORKING_DIRECTORY TEST_LIST_VARIABLE)
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

    set(_add_test_env_mods
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

    file(RELATIVE_PATH _add_test_path ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    string(REPLACE "\\" "/" _add_test_path "${_add_test_path}")

    set(_add_test_target "${_add_test_path}/${_add_test_NAME}")

    set(_add_test_target_list "${_add_test_target}")

    add_test(
        NAME ${_add_test_target}
        COMMAND ${_add_test_COMMAND}
        WORKING_DIRECTORY ${_add_test_WORKING_DIRECTORY}
    )

    if(GENERATE_VSCODE_LAUNCH_JSON)
        message(STATUS "Adding ${_add_test_target} to .vscode/launch.json")

        execute_process(
            COMMAND ${Python_EXECUTABLE} deploy/add-launch-target.py
                --name "${_add_test_target}"
                --command "${_add_test_COMMAND}"
                --environment "${_add_test_env_mods}"
                --bin "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                --cwd "${_add_test_WORKING_DIRECTORY}"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
    endif()

    if(ENABLE_VALGRIND AND NOT _add_test_NO_VALGRIND)

        set(_valgrind_flags ${Valgrind_FLAGS})
        foreach(_supp IN LISTS Valgrind_SUPPRESSION_FILES)
            list(APPEND _valgrind_flags "--suppressions=${_supp}")
        endforeach()

        set(i 1)
        foreach(_tool IN LISTS Valgrind_TOOL_LIST)
            set(_add_test_target_tool "${_add_test_target} (${_tool})")
            list(APPEND _add_test_target_list "${_add_test_target_tool}")

            string(REPLACE "-" "_" _tool_no_dash ${_tool})

            add_test(
                NAME "${_add_test_target_tool}"
                COMMAND ${CMAKE_COMMAND} -E env TEST_INDEX=${i}
                    ${Valgrind_EXECUTABLE} --tool=${_tool} ${_valgrind_flags} ${Valgrind_${_tool_no_dash}_FLAGS} ${_add_test_COMMAND}
                WORKING_DIRECTORY ${_add_test_WORKING_DIRECTORY}
            )

            math(EXPR i "${i}+1")
        endforeach()
        
    endif()

    if (DEFINED _add_test_TEST_LIST_VARIABLE)
        set(${_add_test_TEST_LIST_VARIABLE} "${_add_test_target_list}")
    endif()
    
    set_tests_properties(
        ${_add_test_target_list}
        PROPERTIES
            ENVIRONMENT_MODIFICATION "${_add_test_env_mods}"
            FAIL_REGULAR_EXPRESSION "FAILED"
    )

endmacro()