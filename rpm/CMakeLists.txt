
# (SLW) TODO: GLOBUSLICENSE ?

###
### post_install_script / post_uninstall_script / python_module_install.sh / python_module_remove.sh
###

# (SLW) TODO: HUP_TO_INETD, HUP_TO_INETD, HUP_TO_XINETD_SERVICE
if(NOT WIN32)
    set(HUP_TO_INETD "kill -HUP `/sbin/pidof inetd`")
    set(HUP_TO_XINETD "/etc/rc.d/init.d/xinetd restart")
    set(HUP_TO_XINETD_SERVICE "service xinetd restart")
endif()

# This must be placed in the binary directory as it contains information
# specific to the OS/Compiler we're using
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/post_install_script.in
    ${CMAKE_CURRENT_BINARY_DIR}/post_install_script
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/post_install_script
        post_uninstall_script
        python_module_install.sh
        python_module_remove.sh
    DESTINATION rpm
    PERMISSIONS ${MODE_755}
)

###
### etc/envsyms
###

if(WIN32)
    set(LIBPATH "PATH")
    set(LIBPATH_JOIN ";")
elseif(APPLE)
    set(LIBPATH "DYLD_LIBRARY_PATH")
    set(LIBPATH_JOIN ":")
else()
    set(LIBPATH "LD_LIBRARY_PATH")
    set(LIBPATH_JOIN ":")
endif()

# This must be placed in the binary directory as it contains information
# specific to the OS/Compiler we're using
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/envsyms.in
    ${CMAKE_CURRENT_BINARY_DIR}/envsyms
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/envsyms
    TYPE SYSCONF
)

###
### etc/mdsplus.conf.template
###

install(
    FILES
        mdsplus.conf.template
    TYPE SYSCONF
)

###
### xinetd
###

install(
    FILES
        mdsipd.xinetd
        mdsipsd.xinetd
    DESTINATION rpm
)

###
### systemd
###

install(
    FILES
        mdsip.socket
        mdsip@.service
    DESTINATION rpm
)
