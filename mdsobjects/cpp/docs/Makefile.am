SUBDIRS = .
include ../../../docs/config/doxygen.am

# //////////////////////////////////////////////////////////////////////////// #
# DOXYGEN CONFIG #

DX_DOCDIR = ${srcdir}
DX_CONFIG = ${top_srcdir}/docs/config/doxygen_cpp.cfg
DX_PACKAGE_NAME = mdsobjects_cpp
DX_TITLE = MDS Objects C++
DX_BRIEF = "MDSplus object oriented interface"
DX_DESTDIR = ${DESTDIR}${datadir}/doc/@PACKAGE@/${DX_PACKAGE_NAME}


DX_PAGES = .. \
           ${top_srcdir}/include/mdsobjects.h \
           index.md           


           
DX_EXCLUDE_PAGES = 
                   
DX_TAGFILES = 
#../../docs/docs.tag=../../docs/html

MOSTLYCLEANFILES = $(DX_CLEANFILES)

# //////////////////////////////////////////////////////////////////////////// #
# TARGETS #

html: doxygen-run

pdf:  doxygen-pdf

install-data-local: install-html install-pdf

uninstall-local:
	rm -rf $(DX_DESTDIR)
	
all: html


