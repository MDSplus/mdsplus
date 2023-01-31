
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

    set(LV_LIB ${CMAKE_SOURCE_DIR}/3rd-party-apis/labview/cintools/labview-${_host_cpu}.lib)

else()

    ###
    ### LV
    ###

    add_library(
        LV SHARED
        lv.c
    )

    install(TARGETS LV)

    target_link_libraries(
        LV
        PUBLIC
            MdsShr
    )

    target_include_directories(
        LV PUBLIC ${CMAKE_SOURCE_DIR}/3rd-party-apis/labview/cintools
    )

    set(LV_LIB LV)

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

install(TARGETS MDSobjectsLVShr)

target_link_libraries(
    MDSobjectsLVShr
    PUBLIC
        MdsIpShr
        MdsObjectsCppShr
        ${LV_LIB}
)

if(NOT WIN32)
    set(_lv_name1 "MDSobjectsLVShr")
    set(_lv_name2 "${CMAKE_SHARED_LIBRARY_PREFIX}MDSobjectsLVShr")

    add_custom_command(
        TARGET MDSobjectsLVShr POST_BUILD
        BYPRODUCTS ${_lv_name1} ${_lv_name2}
        COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE_NAME:MDSobjectsLVShr> ${_lv_name1}
        COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE_NAME:MDSobjectsLVShr> ${_lv_name2}
        WORKING_DIRECTORY ${LIB_DIR}
    )

    install(
        FILES
            ${LIB_DIR}/${_lv_name1}
            ${LIB_DIR}/${_lv_name2}
        TYPE LIB
    )
endif()