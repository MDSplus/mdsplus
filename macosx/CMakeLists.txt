
install(
    DIRECTORY scripts
    DESTINATION .
    USE_SOURCE_PERMISSIONS
)

install(
    FILES mdsip.plist
    TYPE SYSCONF
)

install(
    FILES mdsplus_launchconfig
    TYPE BIN
)
