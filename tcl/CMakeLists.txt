
###
### tcl_commands
###

FILE(GLOB
    _source_list
    "tcl_*.c"
)

ADD_LIBRARY(
    tcl_commands SHARED
    ${_source_list}
)

TARGET_LINK_LIBRARIES(
    tcl_commands
    PUBLIC
        MdsServerShr
)

###
### Install
###

INSTALL(
    TARGETS tcl_commands
)

IF(WIN32)
    INSTALL(
        FILES mdstcl.bat
        TYPE BIN
    )
ELSE()
    INSTALL(
        FILES mdstcl
        PERMISSIONS ${MODE_755}
        TYPE BIN
    )
ENDIF()