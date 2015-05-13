
SUBDIRS = \
          ../mdstcpip/docs/ \          
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
           lib_mdsobjects.md

DX_EXCLUDE_PAGES = 

#DX_TAGFILES = $(shell cat ${DX_TAGDIR}/taglist.txt | sed 's/\n//' )

DX_TAGFILES = mdsip.tag \
              mdsobjects/cpp/docs


MOSTLYCLEANFILES = $(DX_CLEANFILES) ${DX_TAGDIR}/taglist.txt

# //////////////////////////////////////////////////////////////////////////// #
# TARGETS #

all: build_taglist

html: doxygen-doc
pdf:  doxygen-pdf

	
build_taglist:
	list='$(SUBDIRS)'
	for subdir in $$list; do echo "Making $$target in $$subdir"; \
	($(am__cd) $$subdir && $(MAKE) $(AM_MAKEFLAGS) build_tag) \
	done;
	for subdir in $$list; do echo "Making $$target in $$subdir"; \
	($(am__cd) $$subdir && $(MAKE) $(AM_MAKEFLAGS) doxygen-run) \
	done;

clean_taglist:
	rm -rf ${DX_TAGDIR}/taglist.txt



