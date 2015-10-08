
include ${top_srcdir}/Makefile.inc
include ${top_srcdir}/testing/testing.am


AM_CFLAGS = @SRBINCLUDE@ $(TEST_CFLAGS)
AM_CXXFLAGS = -Wno-deprecated $(CFLAGS) @CXXFLAGS@ $(TEST_CFLAGS)
AM_LIBS = -L@MAKESHLIBDIR@ $(TEST_LIBS) -lMdsTestShr -lMdsLib -lMdsShr -lTreeShr -lTdiShr -lMdsIpShr


## ////////////////////////////////////////////////////////////////////////// ##
## // TESTS  //////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

TEST_EXTENSIONS = .py .pl
AM_DEFAULT_SOURCE_EXT = .c


C_TESTS = \
          example  \
          example2 \
          example3 \
          example4
          
PY_TESTS = \
           pyex1.py

PL_TESTS =


TESTS = $(C_TESTS) $(PY_TESTS) $(PL_TESTS)

AM_TESTS_ENVIRONMENT = export LD_LIBRARY_PATH=@MAKESHLIBDIR@;
LDADD = $(AM_LIBS) 


bin_PROGRAMS = example3
example3_SOURCES = example3.cpp


#
# VALGRIND: If valgrind was enabled by configuration entry the execution of
# tests will be performed using the valgrind command below. Insert suppressions
# files there.
#
# NOTE: all Suppression suffixed test are not propely tests but programs that
# may rise false positives in valgrind memory check so they may be usefull to
# generate suppressions codes.
#

if USE_VALGRIND
LOG_COMPILER = valgrind
AM_LOG_FLAGS = --quiet --trace-children=yes --error-exitcode=1 \
               --dsymutil=yes --leak-check=yes

#               --suppressions=valgrind-stdlib.supp      \
#               --suppressions=valgrind-dlerr.supp       \
#               --suppressions=valgrind-connection.supp  \
#               --suppressions=valgrind-treenode.supp    
endif


#
# Files produced by tests that must be purged
#
MOSTLYCLEANFILES = test-suite.tap \
                   *.out



## ////////////////////////////////////////////////////////////////////////// ##
## // TARGETS  ////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

.PHONY: tests
tests: $(C_TESTS)


all-local: tests

check_PROGRAMS = $(C_TESTS)
check_SCRIPTS  = $(PY_TESTS) $(PL_TESTS)


