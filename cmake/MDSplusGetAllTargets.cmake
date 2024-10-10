include_guard(GLOBAL)

function(mdsplus_get_all_targets _output_list_variable)

    set(_all_project_dirs "")
    get_property(_stack DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY SUBDIRECTORIES)
    while(_stack)
        list(POP_BACK _stack _dir)
        list(APPEND _all_project_dirs ${_dir})
        
        get_property(_subdirs DIRECTORY ${_dir} PROPERTY SUBDIRECTORIES)
        if(_subdirs)
            list(APPEND stack ${_subdirs})
        endif()
    endwhile()

    foreach(_dir ${_all_project_dirs})
        get_property(_dir_target_list DIRECTORY ${_dir} PROPERTY BUILDSYSTEM_TARGETS)
        list(APPEND _target_list ${_dir_target_list})
    endforeach()

    set(${_output_list_variable} ${_target_list} PARENT_SCOPE)

endfunction()