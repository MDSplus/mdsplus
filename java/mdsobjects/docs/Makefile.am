
include @top_builddir@/Makefile.inc
include ../../../docs/config/doxygen.am

# //////////////////////////////////////////////////////////////////////////// #
# DOXYGEN CONFIG #

DX_CONFIG = ${top_srcdir}/docs/config/doxygen_cpp.cfg
DX_DOCDIR = mdsobjects/java/docs
DX_PACKAGE_NAME = mdsobjects_java
DX_TITLE = MDSplus Java interface
DX_BRIEF = "MDSplus object oriented interface"

DX_PAGES = ../src/main/java/MDSplus \
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
