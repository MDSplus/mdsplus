
include @top_builddir@/Makefile.inc
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
           lib_treeshr.md \
           lib_mdsshr.md \
           lib_tdishr.md \
           lib_mdsip.md \
           lib_mdsobjects.md \
           use_docs.md \
           data_types.md \
           dt_integer.md \
           dt_float.md \
           dt_string.md \
           dt_action.md \
           dt_call.md \
           dt_dimension.md \
           dt_conglom.md \
           dt_dispatch.md \
           dt_function.md \
           dt_ident.md \
           dt_method.md \
           dt_range.md \
           dt_routine.md \
           dt_signal.md \
           dt_window.md \
           dt_with_error.md \
           dt_with_units.md
           
           
DX_EXCLUDE_PAGES = 

DX_TAGFILES = mdsip.tag \
              mdsshr.tag \
              mdsobjects/cpp/docs \
              python/MDSplus/docs \
              mdsobjects/java/docs \
              mdslib/docs \
              javascope/docs


MOSTLYCLEANFILES = $(DX_CLEANFILES) \
                   ${DX_TAGDIR}/taglist.txt \
                   ${DESTDIR}${datadir}/doc/@PACKAGE@/index.html

# //////////////////////////////////////////////////////////////////////////// #
# LOCAL TARGETS #


html: doxygen-run
pdf:  doxygen-pdf



# //////////////////////////////////////////////////////////////////////////// #
# RECURSIVE SUBDIRECTORIES DOCUMENT GENERATION #

# Add here all subdirectories that contain a doxygen documentation Makefile
# 
DX_SUBDIRS = \
          ../mdsshr/docs/ \
          ../mdstcpip/docs/ \
          ../mdsobjects/cpp/docs/ \
          ../python/MDSplus/docs/ \
          ../mdsobjects/java/docs/ \
          ../mdslib/docs/ \
          ../javascope/docs \
          .

build_all:
	list='$(DX_SUBDIRS)'
	for subdir in $$list; do echo "Making $$target in $$subdir"; \
	($(am__cd) $$subdir && $(MAKE) $(AM_MAKEFLAGS) build_tag) \
	done;
	for subdir in $$list; do echo "Making $$target in $$subdir"; \
	($(am__cd) $$subdir && $(MAKE) $(AM_MAKEFLAGS) doxygen-doc) \
	done;


install_all:
	list='$(DX_SUBDIRS)'
	for subdir in $$list; do echo "Making $$target in $$subdir"; \
	($(am__cd) $$subdir && $(MAKE) $(AM_MAKEFLAGS) install_tag) \
	done; \
	for subdir in $$list; do echo "Making $$target in $$subdir"; \
	($(am__cd) $$subdir && $(MAKE) $(AM_MAKEFLAGS) doxygen-install) \
	done;

clean_all:
	list='$(DX_SUBDIRS)'
	for subdir in $$list; do echo "Making $$target in $$subdir"; \
	($(am__cd) $$subdir && $(MAKE) $(AM_MAKEFLAGS) clean) \
	done;


# This is a html main page that resides into the root directory of the web
# server and provides a redirection to the docs directory
#
${DESTDIR}${datadir}/doc/@PACKAGE@/index.html:
	touch $@; \
	echo " <html> <head> <meta http-equiv=\"refresh\" content=\"0; url=./docs/html/index.html\"> </head> </html> " > $@;

main_index: ${DESTDIR}${datadir}/doc/@PACKAGE@/index.html


# The default behavior from here is to build all docs for MDSplus
# subdirectories.
#
all: build_all

install-data-local: install_all main_index
