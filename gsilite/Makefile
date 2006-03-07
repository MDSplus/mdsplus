top_srcdir=..
CVSROOT=":pserver:anonymous@cvs.globus.org:/home/globdev/CVS/globus-packages"
CFLAGS=-g -O2 -fpic -I. -I$(OPENSSL_DIR)/ssl -DOPENSSL_NO_KRB5  -DHAVE_CONFIG_H -D_REENTRANT -Wall -DGLOBUS_BUILTIN -Dflavor=\"gcc32pthr\" -DBUILD_DEBUG
LIBS=-Bstatic -L$(OPENSSL_DIR) -lssl -lcrypto -Bdynamic -lpthread -lc -lltdl -ldl -lm

SOURCES=accept.c \
	accept_delegation.c \
	accept_sec_context.c \
	acquire.c \
	acquire_cred.c \
	compare_name.c \
	delete_sec_context.c \
	display.c \
	display_name.c \
	display_status.c \
	duplicate_name.c \
	export_cred.c \
	get_mic.c \
	globus_callback_threads.c \
	globus_callout.c \
	globus_callout_error.c \
	globus_common.c \
	globus_common_paths.c \
	globus_debug.c \
	globus_error.c \
	globus_error_errno.c \
	globus_error_generic.c \
	globus_error_gssapi.c \
	globus_error_hierarchy.c \
	globus_error_openssl.c \
	globus_extension.c \
	globus_fifo.c \
	globus_gsi_callback.c \
	globus_gsi_callback_data.c \
	globus_gsi_callback_error.c \
	globus_gsi_cert_utils.c \
	globus_gsi_cert_utils_error.c \
	globus_gsi_credential.c \
	globus_gsi_cred_error.c \
	globus_gsi_cred_handle_attrs.c \
	globus_gsi_cred_handle.c \
	globus_gsi_gss_error.c \
	globus_gsi_proxy.c \
	globus_gsi_proxy_error.c \
	globus_gsi_proxy_handle_attrs.c \
	globus_gsi_proxy_handle.c \
	globus_gsi_system_config.c \
	globus_gsi_system_config_error.c \
	globus_gss_assist_error.c \
	globus_gss_assist_module.c \
	globus_handle_table.c \
	globus_hashtable.c \
	globus_i_error_errno.c \
	globus_i_error_generic.c \
	globus_i_error_gssapi.c \
	globus_i_error_openssl.c \
	globus_i_gsi_gss_utils.c \
	globus_libc.c \
	globus_libc_setenv.c \
	globus_list.c \
	globus_memory.c \
	globus_module.c \
	globus_object.c \
	globus_object_cache.c \
	globus_oldgaa_utils.c \
	globus_openssl.c \
	globus_print.c \
	globus_priority_q.c \
	globus_thread_common.c \
	globus_thread_pool.c \
	globus_thread_pthreads.c \
	globus_thread_rmutex.c \
	globus_time.c \
	globus_xio_attr.c \
	globus_xio_driver.c \
	globus_xio_gsi.c \
	globus_xio_handle.c \
	globus_xio_load.c \
	globus_xio_pass.c \
	globus_xio_server.c \
	globus_xio_system_select.c \
	globus_xio_tcp_driver.c \
	globus_xio_timeout.c \
	globus_xio_util.c \
	hostname.c \
	import_name.c \
	init_delegation.c \
	init_sec_context.c \
	inquire_context.c \
	inquire_cred.c \
	module.c \
	oid_functions.c \
	oldgaa_alloc.c \
	oldgaa_api.c \
	oldgaa_gl_internal_err.c \
	oldgaa_policy_evaluator.c \
	oldgaa_release.c \
	oldgaa_utils.c \
	proxycertinfo.c \
	proxypolicy.c \
	release_buffer.c \
	release_cred.c \
	release_name.c \
	rfc1779.c \
	tokens_f.c \
	verify_mic.c \
	wrap.c \
	unwrap.c \
	init.c

OBJECTS = $(SOURCES:.c=.o)

CONFIG_HEADERS = \
	config.h \
	extensions.h \
	globus_config.h \
	globus_dc.h \
	version.h


HEADERS = \
	defs.h \
	e_os.h \
	funcs.h \
	globals.h \
	globus_args.h \
	globus_callback.h \
	globus_callout_constants.h \
	globus_callout.h \
	globus_common.h \
	globus_common_include.h \
	globus_dc.h \
	globus_debug.h \
	globus_error_errno.h \
	globus_error_generic.h \
	globus_error_gssapi.h \
	globus_error.h \
	globus_error_hierarchy.h \
	globus_error_openssl.h \
	globus_error_string.h \
	globus_extension.h \
	globus_fifo.h \
	globus_gridmap_callout_error.h \
	globus_gsi_callback_constants.h \
	globus_gsi_callback.h \
	globus_gsi_cert_utils_constants.h \
	globus_gsi_cert_utils.h \
	globus_gsi_cred_constants.h \
	globus_gsi_credential.h \
	globus_gsi_gss_constants.h \
	globus_gsi_proxy_constants.h \
	globus_gsi_proxy.h \
	globus_gsi_system_config_constants.h \
	globus_gsi_system_config.h \
	globus_gss_assist_constants.h \
	globus_gss_assist.h \
	globus_handle_table.h \
	globus_hashtable.h \
	globus_i_callback.h \
	globus_i_callout.h \
	globus_i_error_errno.h \
	globus_i_error_generic.h \
	globus_i_error_gssapi.h \
	globus_i_error_openssl.h \
	globus_i_gsi_callback.h \
	globus_i_gsi_cert_utils.h \
	globus_i_gsi_credential.h \
	globus_i_gsi_gss_utils.h \
	globus_i_gsi_proxy.h \
	globus_i_gsi_system_config.h \
	globus_i_gss_assist.h \
	globus_io_error_hierarchy.h \
	globus_io.h \
	globus_i_thread.h \
	globus_i_xio_gsi.h \
	globus_i_xio.h \
	globus_libc.h \
	globus_list.h \
	globus_logging.h \
	globus_memory.h \
	globus_module.h \
	globus_nexus.h \
	globus_object_cache.h \
	globus_object.h \
	globus_object_hierarchy.h \
	globus_oldgaa.h \
	globus_oldgaa_utils.h \
	globus_openssl.h \
	globus_print.h \
	globus_priority_q.h \
	globus_range_list.h \
	globus_strptime.h \
	globus_symboltable.h \
	globus_thread_common.h \
	globus_thread_none.h \
	globus_thread_pool.h \
	globus_thread_pthreads.h \
	globus_thread_rmutex.h \
	globus_thread_rw_mutex.h \
	globus_tilde_expand.h \
	globus_time.h \
	globus_url.h \
	globus_uuid.h \
	globus_xio_driver.h \
	globus_xio_gsi.h \
	globus_xio.h \
	globus_xio_load.h \
	globus_xio_system.h \
	globus_xio_tcp_driver.h \
	globus_xio_types.h \
	globus_xio_util.h \
	gssapi.h \
	gssapi_openssl.h \
	internal.h \
	kssl_lcl.h \
	macros.h \
	nexus.h \
	nx_ver.h \
	oldgaa_gl_internal_err.h \
	oldgaa_policy_evaluator.h \
	oldgaa_utils.h \
	proxycertinfo.h \
	proxypolicy.h \
	rfc1779.h \
	types.h


all : ./globus $(top_srcdir)/lib/libGsiLite.so

./globus :
	mkdir ./globus

$(top_srcdir)/lib/libGsiLite.so : $(CONFIG_HEADERS) $(HEADERS) $(OBJECTS)
	ld -o $@ -shared $(OBJECTS) $(LIBS)

clean :
	- rm *.o *.c *.h
	- rm -Rf globus
	- rm $(top_srcdir)/lib/libGsiLite.so

accept.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gss_assist/source/accept.c
	ln -sf globus/gsi/gss_assist/source/accept.c ./

accept_delegation.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/accept_delegation.c
	ln -sf globus/gsi/gssapi/source/library/accept_delegation.c ./

accept_sec_context.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/accept_sec_context.c
	ln -sf globus/gsi/gssapi/source/library/accept_sec_context.c ./

acquire.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gss_assist/source/acquire.c
	ln -sf globus/gsi/gss_assist/source/acquire.c ./

acquire_cred.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/acquire_cred.c
	ln -sf globus/gsi/gssapi/source/library/acquire_cred.c ./

compare_name.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/compare_name.c
	ln -sf globus/gsi/gssapi/source/library/compare_name.c ./

delete_sec_context.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/delete_sec_context.c
	ln -sf globus/gsi/gssapi/source/library/delete_sec_context.c ./

display.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gss_assist/source/display.c
	ln -sf globus/gsi/gss_assist/source/display.c ./

display_name.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/display_name.c
	ln -sf globus/gsi/gssapi/source/library/display_name.c ./

display_status.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/display_status.c
	ln -sf globus/gsi/gssapi/source/library/display_status.c ./

duplicate_name.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/duplicate_name.c
	ln -sf globus/gsi/gssapi/source/library/duplicate_name.c ./

export_cred.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/export_cred.c
	ln -sf globus/gsi/gssapi/source/library/export_cred.c ./

get_mic.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/get_mic.c
	ln -sf globus/gsi/gssapi/source/library/get_mic.c ./

globus_callback_threads.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_callback_threads.c
	ln -sf globus/common/source/library/globus_callback_threads.c ./

globus_callout.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 callout/source/library/globus_callout.c
	ln -sf globus/callout/source/library/globus_callout.c ./

globus_callout_error.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 callout/source/library/globus_callout_error.c
	ln -sf globus/callout/source/library/globus_callout_error.c ./

globus_common.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_common.c
	ln -sf globus/common/source/library/globus_common.c ./

globus_common_paths.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_common_paths.c
	ln -sf globus/common/source/library/globus_common_paths.c ./

globus_debug.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_debug.c
	ln -sf globus/common/source/library/globus_debug.c ./

globus_error.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_error.c
	ln -sf globus/common/source/library/globus_error.c ./

globus_error_errno.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_error_errno.c
	ln -sf globus/common/source/library/globus_error_errno.c ./

globus_error_generic.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_error_generic.c
	ln -sf globus/common/source/library/globus_error_generic.c ./

globus_error_gssapi.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi_error/source/library/globus_error_gssapi.c
	ln -sf globus/gsi/gssapi_error/source/library/globus_error_gssapi.c ./

globus_error_hierarchy.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_error_hierarchy.c
	ln -sf globus/common/source/library/globus_error_hierarchy.c ./

globus_error_openssl.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/openssl_error/source/library/globus_error_openssl.c
	ln -sf globus/gsi/openssl_error/source/library/globus_error_openssl.c ./

globus_extension.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_extension.c
	ln -sf globus/common/source/library/globus_extension.c ./

globus_fifo.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_fifo.c
	ln -sf globus/common/source/library/globus_fifo.c ./

globus_gsi_callback.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/globus_gsi_callback.c
	ln -sf globus/gsi/callback/source/library/globus_gsi_callback.c ./

globus_gsi_callback_data.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/globus_gsi_callback_data.c
	ln -sf globus/gsi/callback/source/library/globus_gsi_callback_data.c ./

globus_gsi_callback_error.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/globus_gsi_callback_error.c
	ln -sf globus/gsi/callback/source/library/globus_gsi_callback_error.c ./

globus_gsi_cert_utils.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/cert_utils/source/library/globus_gsi_cert_utils.c
	ln -sf globus/gsi/cert_utils/source/library/globus_gsi_cert_utils.c ./

globus_gsi_cert_utils_error.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/cert_utils/source/library/globus_gsi_cert_utils_error.c
	ln -sf globus/gsi/cert_utils/source/library/globus_gsi_cert_utils_error.c ./

globus_gsi_credential.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/credential/source/library/globus_gsi_credential.c
	ln -sf globus/gsi/credential/source/library/globus_gsi_credential.c ./

globus_gsi_cred_error.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/credential/source/library/globus_gsi_cred_error.c
	ln -sf globus/gsi/credential/source/library/globus_gsi_cred_error.c ./

globus_gsi_cred_handle_attrs.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/credential/source/library/globus_gsi_cred_handle_attrs.c
	ln -sf globus/gsi/credential/source/library/globus_gsi_cred_handle_attrs.c ./

globus_gsi_cred_handle.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/credential/source/library/globus_gsi_cred_handle.c
	ln -sf globus/gsi/credential/source/library/globus_gsi_cred_handle.c ./

globus_gsi_gss_error.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/globus_gsi_gss_error.c
	ln -sf globus/gsi/gssapi/source/library/globus_gsi_gss_error.c ./

globus_gsi_proxy.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/proxy/proxy_core/source/library/globus_gsi_proxy.c
	ln -sf globus/gsi/proxy/proxy_core/source/library/globus_gsi_proxy.c ./

globus_gsi_proxy_error.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/proxy/proxy_core/source/library/globus_gsi_proxy_error.c
	ln -sf globus/gsi/proxy/proxy_core/source/library/globus_gsi_proxy_error.c ./

globus_gsi_proxy_handle_attrs.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/proxy/proxy_core/source/library/globus_gsi_proxy_handle_attrs.c
	ln -sf globus/gsi/proxy/proxy_core/source/library/globus_gsi_proxy_handle_attrs.c ./

globus_gsi_proxy_handle.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/proxy/proxy_core/source/library/globus_gsi_proxy_handle.c
	ln -sf globus/gsi/proxy/proxy_core/source/library/globus_gsi_proxy_handle.c ./

globus_gsi_system_config.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/sysconfig/source/library/globus_gsi_system_config.c
	ln -sf globus/gsi/sysconfig/source/library/globus_gsi_system_config.c ./

globus_gsi_system_config_error.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/sysconfig/source/library/globus_gsi_system_config_error.c
	ln -sf globus/gsi/sysconfig/source/library/globus_gsi_system_config_error.c ./

globus_gss_assist_error.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gss_assist/source/globus_gss_assist_error.c
	ln -sf globus/gsi/gss_assist/source/globus_gss_assist_error.c ./

globus_gss_assist_module.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gss_assist/source/globus_gss_assist_module.c
	ln -sf globus/gsi/gss_assist/source/globus_gss_assist_module.c ./

globus_handle_table.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_handle_table.c
	ln -sf globus/common/source/library/globus_handle_table.c ./

globus_hashtable.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_hashtable.c
	ln -sf globus/common/source/library/globus_hashtable.c ./

globus_i_error_errno.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_i_error_errno.c
	ln -sf globus/common/source/library/globus_i_error_errno.c ./

globus_i_error_generic.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_i_error_generic.c
	ln -sf globus/common/source/library/globus_i_error_generic.c ./

globus_i_error_gssapi.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi_error/source/library/globus_i_error_gssapi.c
	ln -sf globus/gsi/gssapi_error/source/library/globus_i_error_gssapi.c ./

globus_i_error_openssl.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/openssl_error/source/library/globus_i_error_openssl.c
	ln -sf globus/gsi/openssl_error/source/library/globus_i_error_openssl.c ./

globus_i_gsi_gss_utils.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/globus_i_gsi_gss_utils.c
	ln -sf globus/gsi/gssapi/source/library/globus_i_gsi_gss_utils.c ./

globus_libc.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_libc.c
	ln -sf globus/common/source/library/globus_libc.c ./

globus_libc_setenv.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_libc_setenv.c
	ln -sf globus/common/source/library/globus_libc_setenv.c ./

globus_list.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_list.c
	ln -sf globus/common/source/library/globus_list.c ./

globus_memory.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_memory.c
	ln -sf globus/common/source/library/globus_memory.c ./

globus_module.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_module.c
	ln -sf globus/common/source/library/globus_module.c ./

globus_object.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_object.c
	ln -sf globus/common/source/library/globus_object.c ./

globus_object_cache.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_object_cache.c
	ln -sf globus/common/source/library/globus_object_cache.c ./

globus_oldgaa_utils.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/oldgaa/globus_oldgaa_utils.c
	ln -sf globus/gsi/callback/source/library/oldgaa/globus_oldgaa_utils.c ./

##/home/twf/gt3-newest/packaging/source-trees/gt2-cvs/gsi/ssl_utils/source/library/oldgaa/globus_oldgaa_utils.c

globus_openssl.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/openssl_module/source/library/globus_openssl.c
	ln -sf globus/gsi/openssl_module/source/library/globus_openssl.c ./

globus_print.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_print.c
	ln -sf globus/common/source/library/globus_print.c ./

globus_priority_q.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_priority_q.c
	ln -sf globus/common/source/library/globus_priority_q.c ./

globus_thread_common.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_thread_common.c
	ln -sf globus/common/source/library/globus_thread_common.c ./

globus_thread_pool.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_thread_pool.c
	ln -sf globus/common/source/library/globus_thread_pool.c ./

globus_thread_pthreads.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_thread_pthreads.c
	ln -sf globus/common/source/library/globus_thread_pthreads.c ./

globus_thread_rmutex.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_thread_rmutex.c
	ln -sf globus/common/source/library/globus_thread_rmutex.c ./

globus_time.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_time.c
	ln -sf globus/common/source/library/globus_time.c ./

globus_xio_attr.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/globus_xio_attr.c
	ln -sf globus/xio/src/globus_xio_attr.c ./

globus_xio_driver.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/globus_xio_driver.c
	ln -sf globus/xio/src/globus_xio_driver.c ./

globus_xio_gsi.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/builtins/gsi/globus_xio_gsi.c
	ln -sf globus/xio/src/builtins/gsi/globus_xio_gsi.c ./

globus_xio_handle.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/globus_xio_handle.c
	ln -sf globus/xio/src/globus_xio_handle.c ./

globus_xio_load.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/globus_xio_load.c
	ln -sf globus/xio/src/globus_xio_load.c ./

globus_xio_pass.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/globus_xio_pass.c
	ln -sf globus/xio/src/globus_xio_pass.c ./

globus_xio_server.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/globus_xio_server.c
	ln -sf globus/xio/src/globus_xio_server.c ./

globus_xio_system_select.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/globus_xio_system_select.c
	ln -sf globus/xio/src/globus_xio_system_select.c ./

globus_xio_tcp_driver.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/builtins/tcp/globus_xio_tcp_driver.c
	ln -sf globus/xio/src/builtins/tcp/globus_xio_tcp_driver.c ./

globus_xio_timeout.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/globus_xio_timeout.c
	ln -sf globus/xio/src/globus_xio_timeout.c ./

globus_xio_util.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/globus_xio_util.c
	ln -sf globus/xio/src/globus_xio_util.c ./

hostname.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gss_assist/source/hostname.c
	ln -sf globus/gsi/gss_assist/source/hostname.c ./

import_name.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/import_name.c
	ln -sf globus/gsi/gssapi/source/library/import_name.c ./

init.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gss_assist/source/init.c
	ln -sf globus/gsi/gss_assist/source/init.c ./

##/home/twf/gt3-newest/packaging/source-trees/gt2-cvs/mds/openldap/giis-ldapmodules/init.c
##/home/twf/gt3-newest/packaging/source-trees/gt2-cvs/mds/openldap/ldapmodules/init.c
##/home/twf/gt3-newest/packaging/source-trees/gt2-cvs/nexus/source/nexus/init.c

init_delegation.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/init_delegation.c
	ln -sf globus/gsi/gssapi/source/library/init_delegation.c ./

init_sec_context.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/init_sec_context.c
	ln -sf globus/gsi/gssapi/source/library/init_sec_context.c ./

inquire_context.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/inquire_context.c
	ln -sf globus/gsi/gssapi/source/library/inquire_context.c ./

inquire_cred.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/inquire_cred.c
	ln -sf globus/gsi/gssapi/source/library/inquire_cred.c ./

module.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/module.c
	ln -sf globus/gsi/gssapi/source/library/module.c ./

##/home/twf/gt3-newest/packaging/source-trees/gt2-cvs/gsi/ssl_utils/source/library/module.c
##/home/twf/gt3-newest/packaging/source-trees/gt2-cvs/nexus/source/nexus/module.c

oid_functions.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/oid_functions.c
	ln -sf globus/gsi/gssapi/source/library/oid_functions.c ./

oldgaa_alloc.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/oldgaa/oldgaa_alloc.c
	ln -sf globus/gsi/callback/source/library/oldgaa/oldgaa_alloc.c ./

##/home/twf/gt3-newest/packaging/source-trees/gt2-cvs/gsi/ssl_utils/source/library/oldgaa/oldgaa_alloc.c

oldgaa_api.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/oldgaa/oldgaa_api.c
	ln -sf globus/gsi/callback/source/library/oldgaa/oldgaa_api.c ./

##/home/twf/gt3-newest/packaging/source-trees/gt2-cvs/gsi/ssl_utils/source/library/oldgaa/oldgaa_api.c

oldgaa_gl_internal_err.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/oldgaa/oldgaa_gl_internal_err.c
	ln -sf globus/gsi/callback/source/library/oldgaa/oldgaa_gl_internal_err.c ./

##/home/twf/gt3-newest/packaging/source-trees/gt2-cvs/gsi/ssl_utils/source/library/oldgaa/oldgaa_gl_internal_err.c

oldgaa_policy_evaluator.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/oldgaa/oldgaa_policy_evaluator.c
	ln -sf globus/gsi/callback/source/library/oldgaa/oldgaa_policy_evaluator.c ./

##/home/twf/gt3-newest/packaging/source-trees/gt2-cvs/gsi/ssl_utils/source/library/oldgaa/oldgaa_policy_evaluator.c

oldgaa_release.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/oldgaa/oldgaa_release.c
	ln -sf globus/gsi/callback/source/library/oldgaa/oldgaa_release.c ./

##/home/twf/gt3-newest/packaging/source-trees/gt2-cvs/gsi/ssl_utils/source/library/oldgaa/oldgaa_release.c

oldgaa_utils.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/oldgaa/oldgaa_utils.c
	ln -sf globus/gsi/callback/source/library/oldgaa/oldgaa_utils.c ./

##/home/twf/gt3-newest/packaging/source-trees/gt2-cvs/gsi/ssl_utils/source/library/oldgaa/oldgaa_utils.c

proxycertinfo.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/proxy/proxy_ssl/source/library/proxycertinfo.c
	ln -sf globus/gsi/proxy/proxy_ssl/source/library/proxycertinfo.c ./

proxypolicy.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/proxy/proxy_ssl/source/library/proxypolicy.c
	ln -sf globus/gsi/proxy/proxy_ssl/source/library/proxypolicy.c ./

release_buffer.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/release_buffer.c
	ln -sf globus/gsi/gssapi/source/library/release_buffer.c ./

release_cred.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/release_cred.c
	ln -sf globus/gsi/gssapi/source/library/release_cred.c ./

release_name.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/release_name.c
	ln -sf globus/gsi/gssapi/source/library/release_name.c ./

rfc1779.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/oldgaa/rfc1779.c
	ln -sf globus/gsi/callback/source/library/oldgaa/rfc1779.c ./

##/home/twf/gt3-newest/packaging/source-trees/gt2-cvs/gsi/ssl_utils/source/library/oldgaa/rfc1779.c

tokens_f.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gss_assist/source/tokens_f.c
	ln -sf globus/gsi/gss_assist/source/tokens_f.c ./

unwrap.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/unwrap.c
	ln -sf globus/gsi/gssapi/source/library/unwrap.c ./

##/home/twf/gt3-newest/packaging/source-trees/gt2-cvs/gsi/gss_assist/source/unwrap.c

verify_mic.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/verify_mic.c
	ln -sf globus/gsi/gssapi/source/library/verify_mic.c ./

wrap.c :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/wrap.c
	ln -sf globus/gsi/gssapi/source/library/wrap.c ./

#/home/twf/gt3-newest/packaging/source-trees/gt2-cvs/gsi/gss_assist/source/wrap.c


#config.h :
#	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/config.h
#	ln -sf globus/common/source/config.h ./

config.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/config.h.in
	ln -sf globus/common/source/config.h.in ./
	ln -sf config/config.h ./

defs.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 nexus/source/nexus/defs.h
	ln -sf globus/nexus/source/nexus/defs.h ./

e_os.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/openssl_gpt/e_os.h
	ln -sf globus/gsi/openssl_gpt/e_os.h ./

#extensions.h :
#	ln -sf globus/xio/src/extensions.h ./

extensions.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/extensions.h.in
	ln -sf globus/xio/src/extensions.h.in ./
	ln -sf config/extensions.h ./

funcs.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 nexus/source/nexus/funcs.h
	ln -sf globus/nexus/source/nexus/funcs.h ./

globals.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 nexus/source/nexus/globals.h
	ln -sf globus/nexus/source/nexus/globals.h

globus_args.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_args.h
	ln -sf globus/common/source/library/globus_args.h ./

globus_callback.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_callback.h
	ln -sf globus/common/source/library/globus_callback.h ./

globus_callout_constants.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 callout/source/library/globus_callout_constants.h
	ln -sf globus/callout/source/library/globus_callout_constants.h ./

globus_callout.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 callout/source/library/globus_callout.h
	ln -sf globus/callout/source/library/globus_callout.h ./

globus_common.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_common.h.in
	ln -sf globus/common/source/library/globus_common.h.in ./
	ln -sf config/globus_common.h ./


globus_common_include.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_common_include.h
	ln -sf globus/common/source/library/globus_common_include.h ./

globus_config.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 core/source/globus_config.h.in
	ln -sf globus/core/source/globus_config.h.in ./
	ln -sf config/globus_config.h  ./

globus_dc.h :
	ln -sf config/globus_dc.h ./

globus_debug.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_debug.h
	ln -sf globus/common/source/library/globus_debug.h ./

globus_error_errno.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_error_errno.h
	ln -sf globus/common/source/library/globus_error_errno.h ./

globus_error_generic.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_error_generic.h
	ln -sf globus/common/source/library/globus_error_generic.h ./

globus_error_gssapi.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi_error/source/library/globus_error_gssapi.h
	ln -sf globus/gsi/gssapi_error/source/library/globus_error_gssapi.h ./

globus_error.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_error.h
	ln -sf globus/common/source/library/globus_error.h ./

globus_error_hierarchy.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_error_hierarchy.h
	ln -sf globus/common/source/library/globus_error_hierarchy.h ./

globus_error_openssl.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/openssl_error/source/library/globus_error_openssl.h
	ln -sf globus/gsi/openssl_error/source/library/globus_error_openssl.h ./

globus_error_string.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_error_string.h
	ln -sf globus/common/source/library/globus_error_string.h ./

globus_extension.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_extension.h
	ln -sf globus/common/source/library/globus_extension.h ./

globus_fifo.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_fifo.h
	ln -sf globus/common/source/library/globus_fifo.h ./

globus_gridmap_callout_error.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gridmap_callout/error/globus_gridmap_callout_error.h
	ln -sf globus/gsi/gridmap_callout/error/globus_gridmap_callout_error.h ./

globus_gsi_callback_constants.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/globus_gsi_callback_constants.h
	ln -sf globus/gsi/callback/source/library/globus_gsi_callback_constants.h ./

globus_gsi_callback.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/globus_gsi_callback.h
	ln -sf globus/gsi/callback/source/library/globus_gsi_callback.h ./

globus_gsi_cert_utils_constants.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/cert_utils/source/library/globus_gsi_cert_utils_constants.h
	ln -sf globus/gsi/cert_utils/source/library/globus_gsi_cert_utils_constants.h ./

globus_gsi_cert_utils.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/cert_utils/source/library/globus_gsi_cert_utils.h
	ln -sf globus/gsi/cert_utils/source/library/globus_gsi_cert_utils.h ./

globus_gsi_cred_constants.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/credential/source/library/globus_gsi_cred_constants.h
	ln -sf globus/gsi/credential/source/library/globus_gsi_cred_constants.h ./

globus_gsi_credential.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/credential/source/library/globus_gsi_credential.h
	ln -sf globus/gsi/credential/source/library/globus_gsi_credential.h ./

globus_gsi_gss_constants.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/globus_gsi_gss_constants.h
	ln -sf globus/gsi/gssapi/source/library/globus_gsi_gss_constants.h ./

globus_gsi_proxy_constants.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/proxy/proxy_core/source/library/globus_gsi_proxy_constants.h
	ln -sf globus/gsi/proxy/proxy_core/source/library/globus_gsi_proxy_constants.h ./

globus_gsi_proxy.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/proxy/proxy_core/source/library/globus_gsi_proxy.h
	ln -sf globus/gsi/proxy/proxy_core/source/library/globus_gsi_proxy.h ./

globus_gsi_system_config_constants.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/sysconfig/source/library/globus_gsi_system_config_constants.h
	ln -sf globus/gsi/sysconfig/source/library/globus_gsi_system_config_constants.h ./

globus_gsi_system_config.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/sysconfig/source/library/globus_gsi_system_config.h
	ln -sf globus/gsi/sysconfig/source/library/globus_gsi_system_config.h ./

globus_gss_assist_constants.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gss_assist/source/globus_gss_assist_constants.h
	ln -sf globus/gsi/gss_assist/source/globus_gss_assist_constants.h ./

globus_gss_assist.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gss_assist/source/globus_gss_assist.h
	ln -sf globus/gsi/gss_assist/source/globus_gss_assist.h ./

globus_handle_table.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_handle_table.h
	ln -sf globus/common/source/library/globus_handle_table.h ./

globus_hashtable.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_hashtable.h
	ln -sf globus/common/source/library/globus_hashtable.h ./

globus_i_callback.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_i_callback.h
	ln -sf globus/common/source/library/globus_i_callback.h ./

globus_i_callout.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 callout/source/library/globus_i_callout.h
	ln -sf globus/callout/source/library/globus_i_callout.h ./

globus_i_error_errno.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_i_error_errno.h
	ln -sf globus/common/source/library/globus_i_error_errno.h ./

globus_i_error_generic.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_i_error_generic.h
	ln -sf globus/common/source/library/globus_i_error_generic.h ./

globus_i_error_gssapi.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi_error/source/library/globus_i_error_gssapi.h
	ln -sf globus/gsi/gssapi_error/source/library/globus_i_error_gssapi.h ./

globus_i_error_openssl.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/openssl_error/source/library/globus_i_error_openssl.h
	ln -sf globus/gsi/openssl_error/source/library/globus_i_error_openssl.h ./

globus_i_gsi_callback.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/globus_i_gsi_callback.h
	ln -sf globus/gsi/callback/source/library/globus_i_gsi_callback.h ./

globus_i_gsi_cert_utils.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/cert_utils/source/library/globus_i_gsi_cert_utils.h
	ln -sf globus/gsi/cert_utils/source/library/globus_i_gsi_cert_utils.h ./

globus_i_gsi_credential.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/credential/source/library/globus_i_gsi_credential.h
	ln -sf globus/gsi/credential/source/library/globus_i_gsi_credential.h ./

globus_i_gsi_gss_utils.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/globus_i_gsi_gss_utils.h
	ln -sf globus/gsi/gssapi/source/library/globus_i_gsi_gss_utils.h ./

globus_i_gsi_proxy.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/proxy/proxy_core/source/library/globus_i_gsi_proxy.h
	ln -sf globus/gsi/proxy/proxy_core/source/library/globus_i_gsi_proxy.h ./

globus_i_gsi_system_config.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/sysconfig/source/library/globus_i_gsi_system_config.h
	ln -sf globus/gsi/sysconfig/source/library/globus_i_gsi_system_config.h ./

globus_i_gss_assist.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gss_assist/source/globus_i_gss_assist.h
	ln -sf globus/gsi/gss_assist/source/globus_i_gss_assist.h ./

globus_io_error_hierarchy.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 io/source/library/globus_io_error_hierarchy.h
	ln -sf globus/io/source/library/globus_io_error_hierarchy.h ./

globus_io.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 io/source/library/globus_io.h
	ln -sf globus/io/source/library/globus_io.h ./

globus_i_thread.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_i_thread.h
	ln -sf globus/common/source/library/globus_i_thread.h ./

globus_i_xio_gsi.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/builtins/gsi/globus_i_xio_gsi.h
	ln -sf globus/xio/src/builtins/gsi/globus_i_xio_gsi.h ./

globus_i_xio.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/globus_i_xio.h
	ln -sf globus/xio/src/globus_i_xio.h ./

globus_libc.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_libc.h
	ln -sf globus/common/source/library/globus_libc.h ./

globus_list.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_list.h
	ln -sf globus/common/source/library/globus_list.h ./

globus_logging.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_logging.h
	ln -sf globus/common/source/library/globus_logging.h ./

globus_memory.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_memory.h
	ln -sf globus/common/source/library/globus_memory.h ./

globus_module.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_module.h
	ln -sf globus/common/source/library/globus_module.h ./

globus_nexus.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 nexus/source/nexus/globus_nexus.h
	ln -sf globus/nexus/source/nexus/globus_nexus.h ./

globus_object_cache.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_object_cache.h
	ln -sf globus/common/source/library/globus_object_cache.h ./

globus_object.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_object.h
	ln -sf globus/common/source/library/globus_object.h ./

globus_object_hierarchy.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_object_hierarchy.h
	ln -sf globus/common/source/library/globus_object_hierarchy.h ./

globus_oldgaa.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/oldgaa/globus_oldgaa.h
	ln -sf globus/gsi/callback/source/library/oldgaa/globus_oldgaa.h ./

globus_oldgaa_utils.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/oldgaa/globus_oldgaa_utils.h
	ln -sf globus/gsi/callback/source/library/oldgaa/globus_oldgaa_utils.h ./

globus_openssl.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/openssl_module/source/library/globus_openssl.h
	ln -sf globus/gsi/openssl_module/source/library/globus_openssl.h ./

globus_print.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_print.h
	ln -sf globus/common/source/library/globus_print.h ./

globus_priority_q.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_priority_q.h
	ln -sf globus/common/source/library/globus_priority_q.h ./

globus_range_list.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_range_list.h
	ln -sf globus/common/source/library/globus_range_list.h ./

globus_strptime.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_strptime.h
	ln -sf globus/common/source/library/globus_strptime.h ./

globus_symboltable.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_symboltable.h
	ln -sf globus/common/source/library/globus_symboltable.h ./

globus_thread_common.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_thread_common.h
	ln -sf globus/common/source/library/globus_thread_common.h ./

globus_thread_none.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_thread_none.h
	ln -sf globus/common/source/library/globus_thread_none.h ./

globus_thread_pool.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_thread_pool.h
	ln -sf globus/common/source/library/globus_thread_pool.h ./

globus_thread_pthreads.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_thread_pthreads.h
	ln -sf globus/common/source/library/globus_thread_pthreads.h ./

globus_thread_rmutex.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_thread_rmutex.h
	ln -sf globus/common/source/library/globus_thread_rmutex.h ./

globus_thread_rw_mutex.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_thread_rw_mutex.h
	ln -sf globus/common/source/library/globus_thread_rw_mutex.h ./

globus_tilde_expand.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_tilde_expand.h
	ln -sf globus/common/source/library/globus_tilde_expand.h ./

globus_time.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_time.h
	ln -sf globus/common/source/library/globus_time.h ./

globus_url.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_url.h
	ln -sf globus/common/source/library/globus_url.h ./

globus_uuid.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/library/globus_uuid.h
	ln -sf globus/common/source/library/globus_uuid.h ./

globus_xio_driver.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/globus_xio_driver.h
	ln -sf globus/xio/src/globus_xio_driver.h ./

globus_xio_gsi.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/builtins/gsi/globus_xio_gsi.h
	ln -sf globus/xio/src/builtins/gsi/globus_xio_gsi.h ./

globus_xio.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/globus_xio.h
	ln -sf globus/xio/src/globus_xio.h ./

globus_xio_load.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/globus_xio_load.h
	ln -sf globus/xio/src/globus_xio_load.h ./

globus_xio_system.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/globus_xio_system.h
	ln -sf globus/xio/src/globus_xio_system.h ./

globus_xio_tcp_driver.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/builtins/tcp/globus_xio_tcp_driver.h
	ln -sf globus/xio/src/builtins/tcp/globus_xio_tcp_driver.h ./

globus_xio_types.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/globus_xio_types.h
	ln -sf globus/xio/src/globus_xio_types.h ./

globus_xio_util.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 xio/src/globus_xio_util.h
	ln -sf globus/xio/src/globus_xio_util.h ./

gssapi.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/gssapi.h
	ln -sf globus/gsi/gssapi/source/library/gssapi.h ./

gssapi_openssl.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/gssapi/source/library/gssapi_openssl.h
	ln -sf globus/gsi/gssapi/source/library/gssapi_openssl.h ./

internal.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 nexus/source/nexus/internal.h
	ln -sf globus/nexus/source/nexus/internal.h ./

kssl_lcl.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/openssl_gpt/ssl/kssl_lcl.h
	ln -sf globus/gsi/openssl_gpt/ssl/kssl_lcl.h ./

macros.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 nexus/source/nexus/macros.h
	ln -sf globus/nexus/source/nexus/macros.h ./

nexus.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 nexus/source/nexus/nexus.h
	ln -sf globus/nexus/source/nexus/nexus.h ./

nx_ver.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 nexus/source/nexus/nx_ver.h
	ln -sf globus/nexus/source/nexus/nx_ver.h ./

oldgaa_gl_internal_err.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/oldgaa/oldgaa_gl_internal_err.h
	ln -sf globus/gsi/callback/source/library/oldgaa/oldgaa_gl_internal_err.h ./

oldgaa_policy_evaluator.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/oldgaa/oldgaa_policy_evaluator.h
	ln -sf globus/gsi/callback/source/library/oldgaa/oldgaa_policy_evaluator.h ./

oldgaa_utils.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/oldgaa/oldgaa_utils.h
	ln -sf globus/gsi/callback/source/library/oldgaa/oldgaa_utils.h  ./

proxycertinfo.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/proxy/proxy_ssl/source/library/proxycertinfo.h
	ln -sf globus/gsi/proxy/proxy_ssl/source/library/proxycertinfo.h ./

proxypolicy.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/proxy/proxy_ssl/source/library/proxypolicy.h
	ln -sf globus/gsi/proxy/proxy_ssl/source/library/proxypolicy.h ./

rfc1779.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 gsi/callback/source/library/oldgaa/rfc1779.h
	ln -sf globus/gsi/callback/source/library/oldgaa/rfc1779.h ./

types.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 nexus/source/nexus/types.h
	ln -sf globus/nexus/source/nexus/types.h ./

version.h :
	cd globus; cvs -d $(CVSROOT) checkout -r globus_4_0_1 common/source/version.h.in
	ln -sf globus/common/source/version.h.in ./
	ln -sf config/version.h ./
