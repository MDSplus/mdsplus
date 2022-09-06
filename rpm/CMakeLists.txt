
###
### mdsipd.xinetd / mdsipsd.xinetd
###

# (SLW) TODO: GLOBUSLICENSE ?

INSTALL(
    FILES mdsipd.xinetd mdsipsd.xinetd
    DESTINATION rpm
)

###
### post_install_script / post_uninstall_script / python_module_install.sh / python_module_remove.sh
###

# (SLW) TODO: HUP_TO_INETD, HUP_TO_INETD, HUP_TO_XINETD_SERVICE
IF(NOT WIN32)
    SET(HUP_TO_INETD "kill -HUP `/sbin/pidof inetd`")
    SET(HUP_TO_XINETD "/etc/rc.d/init.d/xinetd restart")
    SET(HUP_TO_XINETD_SERVICE "service xinetd restart")
ENDIF()

CONFIGURE_FILE(
    post_install_script.in
    ${RPM_DIR}/post_install_script
)

INSTALL(
    FILES
        ${RPM_DIR}/post_install_script
        post_uninstall_script
        python_module_install.sh
        python_module_remove.sh
    DESTINATION rpm
    PERMISSIONS ${MODE_755}
)

###
### etc/envsyms
###

CONFIGURE_FILE(
    envsyms.in
    ${ETC_DIR}/envsyms
)

INSTALL(
    FILES ${ETC_DIR}/envsyms
    DESTINATION etc
)

###
### etc/mdsplus.conf.template
###

INSTALL(
    FILES mdsplus.conf.template
    DESTINATION etc
)