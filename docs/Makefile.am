
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


DX_PAGES = index.md \
           lib_mdsip.md \
           lib_mdsobjects.md \
           use_docs.md

DX_EXCLUDE_PAGES = 

DX_TAGFILES = mdsip.tag \
              mdsobjects/cpp/docs


MOSTLYCLEANFILES = $(DX_CLEANFILES) ${DX_TAGDIR}/taglist.txt

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
	


all: build_all

install-data-local: install_all
