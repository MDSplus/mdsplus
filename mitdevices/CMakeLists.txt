
if(TARGET XmdsShr)

    ###
    ### MitDevices
    ###

    add_library(
        MitDevices
        
        # Devices
        dt200.c
        dt_acq16.c
        DevRoutines.c
        a12_gen.c a12.c
        a14_gen.c a14.c
        a3204_gen.c a3204.c
        b2408_gen.c b2408.c
        b3224_gen.c b3224.c
        b5910a_gen.c b5910a.c
        dsp2904_gen.c dsp2904.c
        ec727_gen.c ec727.c
        fera_gen.c fera.c
        h908_gen.c h908.c
        h911_gen.c h911.c
        h912_gen.c h912.c
        hm650_gen.c hm650.c
        hv1440_gen.c hv1440.c
        hv1443_gen.c hv1443.c
        hv4032a1_gen.c hv4032a1.c
        hv4032_gen.c hv4032.c
        idl_gen.c idl.c
        incaa16_gen.c incaa16.c
        incaa4_gen.c incaa4.c
        incaa6_gen.c incaa6.c
        j1819_gen.c j1819.c
        j221_gen.c j221.c
        j412_gen.c j412.c
        joerger_adc_gen.c joerger_adc.c
        joerger_adcp_gen.c joerger_adcp.c
        joerger_cg_gen.c joerger_cg.c
        joerger_dac16_gen.c joerger_dac16.c
        joerger_tr16_gen.c joerger_tr16.c
        joerger_tr612_gen.c joerger_tr612.c
        joerger_tr812_gen.c joerger_tr812.c
        l2232_gen.c l2232.c
        l2256_gen.c l2256.c
        l2415_gen.c l2415.c
        l3512a_gen.c l3512a.c
        l3512_gen.c l3512.c
        l4202_gen.c l4202.c
        l6810a_gen.c l6810a.c
        l6810b_gen.c l6810b.c
        l6810c_gen.c l6810c.c
        l6810_gen.c l6810.c
        l8100_gen.c l8100.c
        l8201_gen.c l8201.c
        l8206_gen.c l8206.c
        l8210_gen.c l8210.c
        l8212_04_gen.c l8212.c
        l8212_08_gen.c
        l8212_16_gen.c
        l8212_32_gen.c
        l8501_gen.c l8501.c
        l8590_gen.c l8590.c
        l8590_mem_gen.c l8590_mem.c
        l8590_sclr_gen.c
        l8818_gen.c l8818.c
        l8828_gen.c l8828.c
        mdsdcl_gen.c mdsdcl.c
        mit__clock_gen.c mit__clock.c
        mit_clock_gen.c mit_clock.c
        mit__dclock_gen.c mit__dclock.c
        mit_dclock_gen.c mit_dclock.c
        mit_decoder_gen.c mit_decoder.c
        mit_encoder_gen.c mit_encoder.c
        mit__gate_gen.c mit__gate.c
        mit_gate_gen.c
        mit_gclock_gen.c mit_gclock.c
        mit_pulse_gen.c mit_pulse-gate.c
        mpb__decoder_gen.c mpb__decoder.c
        paragon_hist_gen.c paragon_hist.c
        paragon_rpt_gen.c paragon_rpt.c
        preamp_gen.c preamp.c
        reticon120_gen.c reticon120.c
        t2812_gen.c
        t2814_gen.c
        t2824_gen.c
        t2825_gen.c
        t2860_gen.c
        t4012_gen.c t4012.c
        umccd_gen.c
        u_of_m_spect_gen.c u_of_m_spect.c

        # Generate Devices
        GenDeviceCallData.c
        GenDeviceCvtFloatCode.c
        GenDeviceCvtIntCode.c
        GenDeviceCvtStringCode.c
        GenDeviceFree.c
        GenDeviceHeadNid.c
        GenDeviceSignal.c
        gen_device_msg.c
    )

    target_link_libraries(
        MitDevices
        PRIVATE
            libMdsdcl
            CamShr
            XmdsShr
    )

    if(TARGET IDL::IDL)
        target_link_libraries(
            MitDevices
            PUBLIC
                IDL::IDL
        )
    endif()

    mdsplus_add_static_copy(MitDevices _static_target)

    install(TARGETS MitDevices ${_static_target})

    # MIT$DEVICES

    set(_output "${CMAKE_SHARED_LIBRARY_PREFIX}MIT$DEVICES${CMAKE_SHARED_LIBRARY_SUFFIX}")

    # (SLW) TODO: Test this insane escape sequence on other systems
    string(REPLACE "$" "\\\$$" _output_escaped "${_output}")

    add_custom_command(
        TARGET MitDevices POST_BUILD
        BYPRODUCTS ${_output_escaped}
        COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE_NAME:MitDevices> ${_output_escaped}
        WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    )

    install(
        FILES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${_output}
        TYPE LIB
    )

    ###
    ### MitDevicesIO
    ###

    add_library(
        MitDevicesIO
        io.c
        curvefit.c
    )

    target_link_libraries(
        MitDevicesIO
        PRIVATE
            MdsMath
    )

    install(TARGETS MitDevicesIO)

    if(APPLE)
        # Fix a namespace collision with FREAD and FWRITE
        target_compile_definitions(
            MitDevicesIO
            PRIVATE
                _POSIX_C_SOURCE
        )
    endif()

    ###
    ### MitDevicesMsg
    ###

    add_library(
        MitDevicesMsg
        mitdevices_msg.c
    )

    install(TARGETS MitDevicesMsg)

    # if(TARGET IDL::IDL AND TARGET Motif::Motif)

    #     ###
    #     ### Idl
    #     ###

    #     add_library(
    #         Idl
    #         callidl.c
    #     )

    #     target_link_libraries(
    #         Idl
    #         PUBLIC
    #             IDL::IDL
    #             Motif::Motif
    #     )

    # endif()

    if(TARGET DC1394::DC1394)

        ###
        ### dc1394_support2
        ###

        # TODO DC1394_SUPPORT / DC1394_SUPPORT2

        add_library(
            dc1394_support2
            dc1394_support2.c
        )

        target_link_libraries(
            dc1394_support2
            PUBLIC
                DC1394::DC1394
        )

        install(TARGETS dc1394_support2)

    endif()

    ###
    ### reboot_dtaq_satelite / dtacq_update_board.sh
    ###

    install(
        FILES
            reboot_dtaq_satelite
            dtaq_update_board.sh
        TYPE BIN
        PERMISSIONS ${MODE_755}
    )

    ###
    ### acq_root_filesystem.tgz / acq_root_filesystem.tgz_ffs
    ###

    file(GLOB_RECURSE
        _dtacq_filesystem
        "acq_root_filesystem/bigffs/*"
        "acq_root_filesystem/bin/*"
        "acq_root_filesystem/cgi-bin/*"
        "acq_root_filesystem/lib/*"
        "acq_root_filesystem/mdsplus/*"
        "acq_root_filesystem/usr/*"
    )

    file(GLOB_RECURSE
        _dtacq_filesystem_ffs
        "acq_root_filesystem/ffs/*"
    )

    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/acq_root_filesystem.tgz
        DEPENDS ${_dtacq_filesystem}
        COMMAND ${CMAKE_COMMAND} -E tar -czf ${CMAKE_BINARY_DIR}/acq_root_filesystem.tgz ${_dtacq_filesystem}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/acq_root_filesystem
    )

    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/acq_root_filesystem.tgz_ffs
        DEPENDS ${_dtacq_filesystem_ffs}
        COMMAND ${CMAKE_COMMAND} -E tar -czf ${CMAKE_BINARY_DIR}/acq_root_filesystem.tgz_ffs ${_dtacq_filesystem_ffs}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/acq_root_filesystem
    )

    add_custom_target(
        generate-acq-root-filesystems ALL
        DEPENDS
            ${CMAKE_BINARY_DIR}/acq_root_filesystem.tgz
            ${CMAKE_BINARY_DIR}/acq_root_filesystem.tgz_ffs
    )

    install(
        FILES
            ${CMAKE_BINARY_DIR}/acq_root_filesystem.tgz
            ${CMAKE_BINARY_DIR}/acq_root_filesystem.tgz_ffs
        TYPE LIB
    )
    
endif()