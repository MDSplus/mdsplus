set(_test_source_list
    camshr_crate_state_fakehw_test.c
)

foreach(_test_source ${_test_source_list})

    cmake_path(GET _test_source STEM _test_name)

    add_executable(
        ${_test_name}
        ${_test_source}
    )

    target_link_libraries(
        ${_test_name}
        PRIVATE
            CamShr
            MdsTestShr
    )

    mdsplus_add_test(
        NAME "${_test_name}"
        COMMAND $<TARGET_FILE:${_test_name}>
    )

endforeach()

find_path(CAMSHR_SQLITE3_INCLUDE_DIR sqlite3.h)
find_library(CAMSHR_SQLITE3_LIBRARY sqlite3)

if(CAMSHR_SQLITE3_INCLUDE_DIR AND CAMSHR_SQLITE3_LIBRARY AND TARGET cts_commands)
    set(_sqlite_test_source_list
        camshr_sqlite_schema_test.c
        camshr_sqlite_migration_test.c
        camshr_sqlite_backend_roundtrip_test.c
        camshr_autoconfig_sqlite_fakeproc_test.c
    )

    foreach(_test_source ${_sqlite_test_source_list})

        cmake_path(GET _test_source STEM _test_name)

        add_executable(
            ${_test_name}
            ${_test_source}
        )

        target_include_directories(
            ${_test_name}
            PRIVATE
                ${CAMSHR_SQLITE3_INCLUDE_DIR}
        )

        target_compile_definitions(
            ${_test_name}
            PRIVATE
                CAMSHR_HAVE_SQLITE3=1
        )

        target_link_libraries(
            ${_test_name}
            PRIVATE
                CamShr
                MdsTestShr
                ${CAMSHR_SQLITE3_LIBRARY}
        )

        if(_test_name STREQUAL "camshr_autoconfig_sqlite_fakeproc_test")
            target_link_libraries(
                ${_test_name}
                PRIVATE
                    cts_commands
            )
            mdsplus_add_test(
                NAME "${_test_name}"
                COMMAND $<TARGET_FILE:${_test_name}>
                ENVIRONMENT_MODIFICATION
                    "CAMSHR_PROC_FILE=set:${CMAKE_CURRENT_SOURCE_DIR}/fixtures/proc_scsi_scsi.txt"
            )
        else()
        mdsplus_add_test(
            NAME "${_test_name}"
            COMMAND $<TARGET_FILE:${_test_name}>
        )
        endif()

    endforeach()
elseif(NOT CAMSHR_SQLITE3_INCLUDE_DIR OR NOT CAMSHR_SQLITE3_LIBRARY)
    message(STATUS "Skipping camshr sqlite tests: sqlite3 not found")
else()
    message(STATUS "Skipping camshr sqlite tests: full CAMSHR build unavailable (cts_commands target missing)")
endif()
