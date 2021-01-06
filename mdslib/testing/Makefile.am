include @top_builddir@/Makefile.inc
include ../../testing/testing.am



AM_CFLAGS = $(TARGET_ARCH) $(WARNFLAGS) $(TEST_CFLAGS)
AM_LDFLAGS = -L@MAKESHLIBDIR@ $(RPATHLINK),@MAKESHLIBDIR@ @LIBS@
LDADD = -lMdsLib -lTdiShr -lTreeShr -lMdsShr -lMdsIpShr $(TEST_LIBS)


## ////////////////////////////////////////////////////////////////////////// ##
## // TESTS  //////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

TEST_EXTENSIONS = .out
AM_DEFAULT_SOURCE_EXT = .c

TESTS = \
	dtype_test\
	mdslib_ctest\
	mdslib_ftest\
	mdslib_fremotetest

VALGRIND_SUPPRESSIONS_FILES = 

#
# Files produced by tests that must be purged
#
MOSTLYCLEANFILES = *.out test_*.tree test_*.characteristics test_*.datafile

## ////////////////////////////////////////////////////////////////////////// ##
## // TARGETS  ////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

print_shell:
	@echo "SHELL = ${SHELL}"

all-local: $(TESTS)
clean-local: clean-local-tests

check_PROGRAMS = $(TESTS)
check_SCRIPTS = 

mdslib_ftest$(EXEEXT) : mdslib_ftest.f $(MdsLib)
	$(LINK.F) $(OUTPUT_OPTION) $< @FEXECLIBDIR@@MAKESHLIBDIR@ $(FCFLAGS) $(LDADD) -Wno-error

mdslib_fremotetest$(EXEEXT) : mdslib_fremotetest.f $(MdsLib)
	$(LINK.F) $(OUTPUT_OPTION) $< @FEXECLIBDIR@@MAKESHLIBDIR@ $(FCFLAGS) $(LDADD) -Wno-error

