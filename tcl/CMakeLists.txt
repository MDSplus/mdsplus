
###
### tcl_commands
###

add_library(
    tcl_commands
    tcl_p.h
    tcl_add_node.c
    tcl_add_tag.c
    tcl_clean_datafile.c
    tcl_close.c
    tcl_compress_datafile.c
    tcl_create_pulse.c
    tcl_decompile.c
    tcl_delete_node.c
    tcl_delete_pulse.c
    tcl_directory_tag.c
    tcl_directory.c
    tcl_dispatch.c
    tcl_do_method.c
    tcl_do_node.c
    tcl_edit.c
    tcl_help_device.c
    tcl_put_expression.c
    tcl_remove_tag.c
    tcl_rename.c
    tcl_set_callbacks.c
    tcl_set_current.c
    tcl_set_default.c
    tcl_set_node.c
    tcl_set_readonly.c
    tcl_set_tree.c
    tcl_set_view.c
    tcl_setevent.c
    tcl_setshow_attribute.c
    tcl_setshow_versions.c
    tcl_show_current.c
    tcl_show_data.c
    tcl_show_db.c
    tcl_verify_tree.c
    tcl_wfevent.c
    tcl_write.c
)

target_link_libraries(
    tcl_commands
    PUBLIC
        MdsServerShr
)

install(TARGETS tcl_commands)

###
### Scripts
###

if(WIN32)
    set(_ext ".bat")
endif()

set(_script_list
    mdstcl${_ext}
)

file(COPY ${_script_list}
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

install(
    FILES ${_script_list}
    PERMISSIONS ${MODE_755}
    TYPE BIN
)