SUBDIRS = img .

include @top_builddir@/Makefile.inc
include ../../docs/config/doxygen.am


# //////////////////////////////////////////////////////////////////////////// #
# DOXYGEN CONFIG #
DX_CONFIG = "${top_srcdir}/docs/config/doxygen.cfg"
DX_DOCDIR = mdstcpip/docs
DX_PACKAGE_NAME = mdsip
DX_TITLE = MDSip
DX_BRIEF = "MDSplus remote data access library"


DX_PAGES = .. \
           index.md \
           Clients_connection.md \
           Objects_bindings.md \
           Protocols_plugins.md


DX_EXCLUDE_PAGES =  \
                   ../io_routines/* \
                   ../udt4/* \
                   ../zlib/* \
                   ../mdsip.c \
                   ../mdsip.h \
                   ../mdsip_service.c

DX_TAGFILES = docs.tag


MOSTLYCLEANFILES = $(DX_CLEANFILES)

# //////////////////////////////////////////////////////////////////////////// #
# TARGETS #

all: html

html: doxygen-run
pdf:  doxygen-pdf

install-data-local: doxygen-install
