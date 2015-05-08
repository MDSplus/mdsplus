
SUBDIRS = \
          ../mdstcpip/docs/ \
          ../mdsobjects/cpp/docs/ \
          .

include config/doxygen.am


# //////////////////////////////////////////////////////////////////////////// #
# DOXYGEN CONFIG #

DX_DOCDIR = ${srcdir}
DX_CONFIG = ${top_srcdir}/docs/config/doxygen.cfg
DX_PACKAGE_NAME = docs
DX_TITLE = MDSplus
DX_BRIEF = "online code documentation"
DX_DESTDIR = ${DESTDIR}${datadir}/doc/@PACKAGE@/${DX_PACKAGE_NAME}


DX_PAGES = index.md \
           lib_mdsip.md \
           lib_mdsobjects.md

DX_EXCLUDE_PAGES = 

DX_TAGFILES = \
              ../mdstcpip/docs/mdsip.tag=../../mdsip/html \
              ../mdsobjects/cpp/docs/mdsobjects_cpp.tag=../../mdsobjects_cpp/html

MOSTLYCLEANFILES = $(DX_CLEANFILES)

# //////////////////////////////////////////////////////////////////////////// #
# TARGETS #

html: doxygen-run

pdf:  doxygen-pdf

install-data-local: install-html install-pdf

uninstall-local:
	rm -rf $(DX_DESTDIR)
	
all: html


