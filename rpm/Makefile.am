etcdir = $(exec_prefix)/etc
etc_DATA = envsyms
dist_etc_DATA = mdsplus.conf.template

rpmdir = $(exec_prefix)/rpm
rpm_SCRIPTS = post_uninstall_script
dist_rpm_SCRIPTS = post_install_script python_module_install.sh python_module_remove.sh

if GLOBUSLICENSE
dist_rpm_DATA = fusiongrid-mdsip.xinetd fusiongrid-mdsips.xinetd globus-gatekeeper.xinetd globus-gridftp.xinetd
else
dist_rpm_DATA = mdsipd.xinetd mdsipsd.xinetd mdsip.socket mdsip@.service
endif
