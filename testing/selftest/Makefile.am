
include @top_builddir@/Makefile.inc
include @top_srcdir@/testing/testing.am

AM_CFLAGS = $(TARGET_ARCH) $(TEST_CFLAGS)
AM_CXXFLAGS = $(TARGET_ARCH) -Wno-deprecated @CXXFLAGS@ $(TEST_CFLAGS)
AM_LDFLAGS = -L@MAKESHLIBDIR@ $(RPATHLINK),@MAKESHLIBDIR@ $(TEST_LIBS) $(AM_LIBS)


## ////////////////////////////////////////////////////////////////////////// ##
## // TESTS  //////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

TEST_EXTENSIONS = .py .pl
AM_DEFAULT_SOURCE_EXT = .c


#          example  

C_TESTS = \
          example2 \
          example3 \
          example4
          
PY_TESTS = \
           pyex1.py

PL_TESTS =


TESTS = $(C_TESTS) $(PY_TESTS) $(PL_TESTS)


AM_TESTS_ENVIRONMENT = export LD_LIBRARY_PATH=@MAKESHLIBDIR@;


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

endif


#
# Files produced by tests that must be purged
#
MOSTLYCLEANFILES = *.out


## ////////////////////////////////////////////////////////////////////////// ##
## // TARGETS  ////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##


all-local: $(TESTS)
clean-local: clean-local-tests                    

check_PROGRAMS = $(C_TESTS)
check_SCRIPTS  = $(PY_TESTS) $(PL_TESTS)


