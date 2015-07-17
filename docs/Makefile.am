
SUBDIRS = \
          ../mdstcpip/docs/ \
          ../mdsobjects/cpp/docs/ \
          .


include config/doxygen.am


# //////////////////////////////////////////////////////////////////////////// #
# DOXYGEN CONFIG #

DX_CONFIG = "${top_srcdir}/docs/config/doxygen.cfg"
DX_DOCDIR = docs
DX_PACKAGE_NAME = docs
DX_TITLE = MDSplus
DX_BRIEF = "online code documentation"


DX_PAGES = \
           index.md \
           mdsplus_components.md \
           lib_mdsip.md \
           lib_mdsobjects.md \
           use_docs.md \
           data_types.md \
           dt_integer.md \
           dt_float.md \
           dt_string.md \
           dt_action.md

DX_EXCLUDE_PAGES = 

DX_TAGFILES = mdsip.tag \
              mdsobjects/cpp/docs


MOSTLYCLEANFILES = $(DX_CLEANFILES) \
                   ${DX_TAGDIR}/taglist.txt \
                   ${DESTDIR}${datadir}/doc/@PACKAGE@/index.html

# //////////////////////////////////////////////////////////////////////////// #
# TARGETS #


html: doxygen-run
pdf:  doxygen-pdf


	
build_all:
	list='$(SUBDIRS)'
	for subdir in $$list; do echo "Making $$target in $$subdir"; \
	($(am__cd) $$subdir && $(MAKE) $(AM_MAKEFLAGS) build_tag) \
	done;
	for subdir in $$list; do echo "Making $$target in $$subdir"; \
	($(am__cd) $$subdir && $(MAKE) $(AM_MAKEFLAGS) doxygen-doc) \
	done;


install_all:
	list='$(SUBDIRS)'
	for subdir in $$list; do echo "Making $$target in $$subdir"; \
	($(am__cd) $$subdir && $(MAKE) $(AM_MAKEFLAGS) install_tag) \
	done;
	for subdir in $$list; do echo "Making $$target in $$subdir"; \
	($(am__cd) $$subdir && $(MAKE) $(AM_MAKEFLAGS) doxygen-install) \
	done;	


${DESTDIR}${datadir}/doc/@PACKAGE@/index.html:
	touch $@; \
	echo " <html> <head> <meta http-equiv=\"refresh\" content=\"0; url=./docs/html/index.html\"> </head> </html> " > $@;

main_index: ${DESTDIR}${datadir}/doc/@PACKAGE@/index.html


all: build_all

install-data-local: install_all main_index
