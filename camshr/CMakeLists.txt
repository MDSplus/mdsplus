
if(HAVE_SCSI_SG_H AND NOT MSVC)

    ###
    ### CamShr
    ###

    set(MAJOR 1)
    set(MINOR 8)

    add_library(
        CamShr
        add_entry.c
        bisearch.c
        cam_functions.c
        check_for_file.c
        check_sema4.c
        commit_entry.c
        compare_str.c
        copy.c
        create_sema4.c
        create_tmp_file.c
        debug.c
        expand_db.c
        get_crate_status.c
        get_db_file_size.c
        get_file_count.c
        get_file_name.c
        get_scsi_device_number.c
        global.c
        issort.c
        lock_file.c
        lookup_entry.c
        map_data_file.c
        map_scsi_device.c
        msg.c
        parse_crate_db.c
        parse_cts_db.c
        QueryHighwayType.c
        remove_entry.c
        scsi_io.c
        ScsiSystemStatus.c
        turn_crate_on_off_line.c
        unlock_file.c
        xlate_logicalname.c
        RemCamMulti.c
        RemCamSingle.c
        RemCamIosb.c
    )

    install(TARGETS CamShr)

    target_link_options(
        CamShr
        PRIVATE
            -nostartfiles
    )

    set_target_properties(
        CamShr
        PROPERTIES
            # Creates symlink for .so -> .so.MAJOR.MINOR
            VERSION "${MAJOR}.${MINOR}"
            # Creates symlink for .so -> .so.MAJOR
            SOVERSION "${MAJOR}"
    )

    ###
    ### cts_commands
    ###

    add_library(
        cts_commands
        verbs.c
    )

    install(TARGETS cts_commands)

    target_link_libraries(
        cts_commands
        PUBLIC
            Mdsdcl
            CamShr
    )

    ###
    ### Scripts
    ###

    install(
        FILES cts RENAME mdscts
        PERMISSIONS ${MODE_755}
        TYPE BIN
    )

endif()