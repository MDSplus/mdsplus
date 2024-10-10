
if(ENABLE_LABVIEW)

    install(
        DIRECTORY
            MDSplus
            MDSplus_LV2012
            MDSplus_LV2015
        DESTINATION LabView
        USE_SOURCE_PERMISSIONS
    )

    if(WIN32)

        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            SET(_host_cpu "i686")
        else()
            SET(_host_cpu "x86_64")
        endif()

        add_library(LV INTERFACE IMPORTED)

        set_target_properties(
            LV
            PROPERTIES
                INTERFACE_LINK_LIBRARIES "${_3rd_party_apis}/labview/cintools/labview-${_host_cpu}.lib"
                INTERFACE_INCLUDE_DIRECTORIES "${_3rd_party_apis}/labview/cintools"
        )

    else()

        ###
        ### LV
        ###

        add_library(
            LV
            lv.c
        )

        target_link_libraries(
            LV
            PUBLIC
                MdsShr
        )

        if(LABVIEW_DIR)
            
            target_include_directories(
                LV
                PUBLIC
                    ${LABVIEW_DIR}
            )

        else()
            
            target_include_directories(
                LV
                PUBLIC
                    ${_3rd_party_apis}/labview/cintools
            )

        endif()

        install(TARGETS LV)

    endif()

    ###
    ### MDSobjectsLVShr
    ###

    add_library(
        MDSobjectsLVShr
        mdsdataobjectswrp.cpp
        mdseventobjectswrp.cpp
        mdsipobjectswrp.cpp
        mdstreeobjectswrp.cpp
    )

    target_link_libraries(
        MDSobjectsLVShr
        PUBLIC
            MdsIpShr
            MdsObjectsCppShr
            LV
    )

    install(TARGETS MDSobjectsLVShr)

    if(NOT WIN32)
        set(_lv_name1 "MDSobjectsLVShr")
        set(_lv_name2 "${CMAKE_SHARED_LIBRARY_PREFIX}MDSobjectsLVShr")

        add_custom_command(
            TARGET MDSobjectsLVShr POST_BUILD
            BYPRODUCTS ${_lv_name1} ${_lv_name2}
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:MDSobjectsLVShr> ${_lv_name1}
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:MDSobjectsLVShr> ${_lv_name2}
            WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        )

        install(
            FILES
                ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${_lv_name1}
                ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${_lv_name2}
            TYPE LIB
        )
    endif()

endif()