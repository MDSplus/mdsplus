
include @top_builddir@/Makefile.inc
include ../../../docs/config/doxygen.am

# //////////////////////////////////////////////////////////////////////////// #
# DOXYGEN CONFIG #

# DX_DOCDIR = mdsobjects/cpp/docs
DX_CONFIG = ${top_srcdir}/docs/config/doxygen_cpp.cfg
DX_DOCDIR = mdsobjects/cpp/docs
DX_PACKAGE_NAME = mdsobjects_cpp
DX_TITLE = MDSplus C++ interface
DX_BRIEF = "MDSplus object oriented interface"

DX_PAGES = .. \
           ${top_srcdir}/include/mdsobjects.h \
           ${top_srcdir}/include/usagedef.h \
           ${top_srcdir}/include/mdsplus/ \
           index.md \
           native_types.md \
           wrapped_types.md

DX_EXCLUDE_PAGES = 
                   
DX_TAGFILES = docs.tag

MOSTLYCLEANFILES = $(DX_CLEANFILES)

# //////////////////////////////////////////////////////////////////////////// #
# TARGETS #

all: html

html: doxygen-run
pdf:  doxygen-pdf

install-data-local: doxygen-install
