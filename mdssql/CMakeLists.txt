
if(MINGW AND NOT TARGET Sybase::Sybase)

    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(_ntwdblib "${CMAKE_CURRENT_LIST_DIR}/libntwdblib64.a")
    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86")
        set(_ntwdblib "${CMAKE_CURRENT_LIST_DIR}/libntwdblib32.a")
    endif()

    if(EXISTS _ntwdblib)
        add_library(Sybase::Sybase INTERFACE IMPORTED)
        
        set_target_properties(
            Sybase::Sybase
            PROPERTIES
                INTERFACE_COMPILE_DEFINITIONS "SYBASE"
                INTERFACE_LINK_LIBRARIES "${_ntwdblib}"
        )
    endif()

endif()

if(TARGET Sybase::Sybase)

    ###
    ### MdsSql
    ###

    add_library(
        MdsSql
        dynamic_syb.c
        dbstubs.c
    )

    target_link_libraries(
        MdsSql
        PUBLIC
            Sybase::Sybase
    )

    mdsplus_add_static_copy(MdsSql _static_target)

    install(TARGETS MdsSql ${_static_target})

endif()
