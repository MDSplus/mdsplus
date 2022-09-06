
INSTALL(
    DIRECTORY
        MDSplus
        MDSplus_LV2012
        MDSplus_LV2015
    DESTINATION LabView
    USE_SOURCE_PERMISSIONS
)

IF(WIN32)
    IF(CMAKE_SIZEOF_VOID_P EQUAL 4)
        SET(_host_cpu "i686")
    ELSE()
        SET(_host_cpu "x86_64")
    ENDIF()

    SET(LV_LIB ${CMAKE_SOURCE_DIR}/3rd-party-apis/labview/cintools/labview-${_host_cpu}.lib)
ELSE()

    ###
    ### LV
    ###

    ADD_LIBRARY(
        LV SHARED
        lv.c
    )

    INSTALL(TARGETS LV)

    TARGET_LINK_LIBRARIES(
        LV
        PUBLIC
            MdsShr
    )

    TARGET_INCLUDE_DIRECTORIES(
        LV PUBLIC ${CMAKE_SOURCE_DIR}/3rd-party-apis/labview/cintools
    )

    SET(LV_LIB LV)
    
ENDIF()

###
### MDSobjectsLVShr
###

ADD_LIBRARY(
    MDSobjectsLVShr SHARED
    mdsdataobjectswrp.cpp
    mdseventobjectswrp.cpp
    mdsipobjectswrp.cpp
    mdstreeobjectswrp.cpp
)

INSTALL(TARGETS MDSobjectsLVShr)

TARGET_LINK_LIBRARIES(
    MDSobjectsLVShr
    PUBLIC
        MdsIpShr
        MdsObjectsCppShr
        ${LV_LIB}
)

IF(NOT WIN32)
    SET(_lv_name1 "MDSobjectsLVShr")
    SET(_lv_name2 "${CMAKE_SHARED_LIBRARY_PREFIX}MDSobjectsLVShr")

    ADD_CUSTOM_COMMAND(
        TARGET MDSobjectsLVShr POST_BUILD
        BYPRODUCTS ${_lv_name1} ${_lv_name2}
        COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE_NAME:MDSobjectsLVShr> ${_lv_name1}
        COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE_NAME:MDSobjectsLVShr> ${_lv_name2}
        WORKING_DIRECTORY ${LIB_DIR}
    )

    INSTALL(
        FILES
            ${LIB_DIR}/${_lv_name1}
            ${LIB_DIR}/${_lv_name2}
        TYPE LIB
    )
ENDIF()