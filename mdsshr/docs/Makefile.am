
include @top_builddir@/Makefile.inc
include ../../docs/config/doxygen.am


# //////////////////////////////////////////////////////////////////////////// #
# DOXYGEN CONFIG #
DX_CONFIG = "${top_srcdir}/docs/config/doxygen.cfg"
DX_DOCDIR = mdsshr/docs
DX_PACKAGE_NAME = mdsshr
DX_TITLE = MdsShr
DX_BRIEF = "MDSplus MdsShr kernel library"


DX_PAGES = .. \
           index.md \
           $(top_srcdir)/include/mdsshr.h \
           $(top_srcdir)/include/mdsshr_messages.h \
           $(top_srcdir)/include/mdsdescrip.h \
           $(top_srcdir)/include/mdstypes.h \
           $(top_srcdir)/include/libroutines.h \
           $(top_srcdir)/include/strroutines.h

DX_TAGFILES = docs.tag


MOSTLYCLEANFILES = $(DX_CLEANFILES)

# //////////////////////////////////////////////////////////////////////////// #
# TARGETS #

all: html

html: doxygen-run
pdf:  doxygen-pdf

install-data-local: doxygen-install
