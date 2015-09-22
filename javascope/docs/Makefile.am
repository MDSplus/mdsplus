SUBDIRS = .
include ../../../docs/config/doxygen.am

# //////////////////////////////////////////////////////////////////////////// #
# DOXYGEN CONFIG #

# DX_DOCDIR = mdsobjects/cpp/docs
DX_CONFIG = ${top_srcdir}/docs/config/doxygen_cpp.cfg
DX_DOCDIR = javascope/docs
DX_PACKAGE_NAME = mdsobjects_cpp
DX_TITLE = jScope
DX_BRIEF = "MDSplus java signal viewer"

DX_PAGES = .. \
           ../jScope \
           index.md 

DX_EXCLUDE_PAGES = 
                   
DX_TAGFILES = docs.tag

MOSTLYCLEANFILES = $(DX_CLEANFILES)

# //////////////////////////////////////////////////////////////////////////// #
# TARGETS #

all: html

html: doxygen-run
pdf:  doxygen-pdf

install-data-local: doxygen-install
