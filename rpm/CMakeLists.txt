
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

configure_file(
    post_install_script.in
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

# (SLW) TODO:
set(LIBPATH "LD_LIBRARY_PATH")


configure_file(
    envsyms.in
    ${CMAKE_CURRENT_BINARY_DIR}/envsyms
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/envsyms
    TYPE SYSCONF
)

###
### etc/mdsplus.conf.template
###

install(
    FILES mdsplus.conf.template
    TYPE SYSCONF
)

###
### xinetd
###

install(
    FILES mdsipd.xinetd mdsipsd.xinetd
    DESTINATION rpm
)

###
### systemd
###

install(
    FILES mdsip.socket mdsip@.service
    DESTINATION rpm
)
