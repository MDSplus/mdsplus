
include ../../docs/config/doxygen.am


# //////////////////////////////////////////////////////////////////////////// #
# DOXYGEN CONFIG #
DX_CONFIG = "${top_srcdir}/docs/config/doxygen.cfg"
DX_DOCDIR = mdslib/docs
DX_PACKAGE_NAME = mdslib
DX_TITLE = MDSlib
DX_BRIEF = "MDSplus library"


DX_PAGES = .. \
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
