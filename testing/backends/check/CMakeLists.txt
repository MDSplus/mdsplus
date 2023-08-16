
###
### Check
###

add_library(
    Check STATIC
    check_error.c
    check_list.c
	check_log.c
	check_msg.c
	check_pack.c
	check_print.c
	check_run.c
	check_str.c
	check.c 
)

target_include_directories(
    Check
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(
    Check
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

if (APPLE)
    target_compile_definitions(
        Check
        PUBLIC
            _XOPEN_SOURCE=700
    )
endif()

target_link_libraries(
    Check
    PUBLIC
        $<$<BOOL:${LIBRT}>:${LIBRT}>
        Threads::Threads
        LibM::LibM
)
