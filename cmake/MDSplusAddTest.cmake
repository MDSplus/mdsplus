include_guard(GLOBAL)

#
# mdsplus_add_test(NAME <name>
#                  COMMAND <command> [<arg>...])
#                  [WORKING_DIRECTORY <dir>]
#                  [ENVIRONMENT_MODIFICATIONS <mods>]
#                  [NO_VALGRIND])
#
# Call add_test() and set_tests_properties() to configure all of the test's properties.
#
# If ENABLE_VALGRIND=ON and NO_VALGRIND is not set, then another test will be added
# for each tool in Valgrind_TOOL_LIST.
#
# If GENERATE_VSCODE_LAUNCH_JSON=ON, then deploy/add-launch-target.py will be called
# for each test as well.
#
# https://cmake.org/cmake/help/latest/command/add_test.html
# https://cmake.org/cmake/help/latest/prop_test/ENVIRONMENT_MODIFICATION.html
#
function(mdsplus_add_test)

    cmake_parse_arguments(
        PARSE_ARGV 0 ARGS
        # Booleans
        "NO_VALGRIND"
        # Single-Value
        "NAME;WORKING_DIRECTORY;TEST_LIST_VARIABLE"
        # Multi-Value
        "COMMAND;ENVIRONMENT_MODIFICATION"
    )

    if(NOT DEFINED ARGS_WORKING_DIRECTORY)
        set(ARGS_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endif()

    set(_env_mods
        # Used to find the rest of MDSplus
        "MDSPLUS_DIR=set:${CMAKE_SOURCE_DIR}"
        "MDSPLUS_LIBRARY_PATH=set:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"

        # Used to search for TDI and Python scripts, and to add TDI drivers
        "MDS_PATH=set:${CMAKE_SOURCE_DIR}/tdi"
        
        # Some tests define their own TDI functions
        "MDS_PATH=cmake_list_prepend:${CMAKE_CURRENT_SOURCE_DIR}"

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
    )

    if(WIN32)
        # Windows searches $PATH for loading .dll's
        list(APPEND _env_mods
            "PATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
        )
    else()
        # Linux searches $LD_LIBRARY_PATH for loading .so's
        # This is set for Apple as well for backwards compatibility
        list(APPEND _env_mods
            "LD_LIBRARY_PATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
        )

        if(APPLE)
            # Apple searches $DYLD_LIBRARY_PATH for loading .dylib's
            list(APPEND _env_mods
                "DYLD_LIBRARY_PATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
            )
        endif()
    endif()

    list(APPEND _env_mods ${ARGS_ENVIRONMENT_MODIFICATION})

    file(RELATIVE_PATH _path ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    string(REPLACE "\\" "/" _path "${_path}")

    set(_target "${_path}/${ARGS_NAME}")

    set(_target_list "${_target}")

    add_test(
        NAME ${_target}
        COMMAND ${ARGS_COMMAND}
        WORKING_DIRECTORY ${ARGS_WORKING_DIRECTORY}
    )

    if(GENERATE_VSCODE_LAUNCH_JSON)
        message(STATUS "Adding ${_target} to .vscode/launch.json")

        execute_process(
            COMMAND ${Python_EXECUTABLE} deploy/add-launch-target.py
                --name "${_target}"
                --command "${ARGS_COMMAND}"
                --environment "${_env_mods}"
                --bin "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                --cwd "${ARGS_WORKING_DIRECTORY}"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
    endif()

    if(ENABLE_VALGRIND AND NOT ARGS_NO_VALGRIND)

        set(_valgrind_flags ${Valgrind_FLAGS})
        foreach(_supp IN LISTS Valgrind_SUPPRESSION_FILES)
            list(APPEND _valgrind_flags "--suppressions=${_supp}")
        endforeach()

        set(i 1)
        foreach(_tool IN LISTS Valgrind_TOOL_LIST)
            set(_target_tool "${_target} (${_tool})")
            list(APPEND _target_list "${_target_tool}")

            string(REPLACE "-" "_" _tool_no_dash ${_tool})

            add_test(
                NAME "${_target_tool}"
                COMMAND ${CMAKE_COMMAND} -E env TEST_INDEX=${i}
                    ${Valgrind_EXECUTABLE} --tool=${_tool} ${_valgrind_flags} ${Valgrind_${_tool_no_dash}_FLAGS} ${ARGS_COMMAND}
                WORKING_DIRECTORY ${ARGS_WORKING_DIRECTORY}
            )

            math(EXPR i "${i}+1")
        endforeach()
        
    endif()

    if (DEFINED ARGS_TEST_LIST_VARIABLE)
        set(${ARGS_TEST_LIST_VARIABLE} "${_target_list}" PARENT_SCOPE)
    endif()
    
    set_tests_properties(
        ${_target_list}
        PROPERTIES
            ENVIRONMENT_MODIFICATION "${_env_mods}"
            FAIL_REGULAR_EXPRESSION "FAILED"
    )

endfunction()