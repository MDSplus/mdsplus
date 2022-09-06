
###
### Check
###

ADD_LIBRARY(
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

TARGET_INCLUDE_DIRECTORIES(
    Check
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

SET_TARGET_PROPERTIES(
    Check
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

TARGET_LINK_LIBRARIES(
    Check
    PUBLIC
        ${LIBRT}
)