SUBDIRS = testutils .

include ${top_srcdir}/Makefile.inc
include ${top_srcdir}/testing/testing.am


AM_CFLAGS = @SRBINCLUDE@ $(TEST_CFLAGS)
AM_CXXFLAGS = -Wno-deprecated $(CFLAGS) @CXXFLAGS@ $(TEST_CFLAGS)
AM_LIBS = -L@MAKESHLIBDIR@ $(TEST_LIBS) -lMdsShr -lTreeShr -lTdiShr -lMdsIpShr -lMdsObjectsCppShr 


## ////////////////////////////////////////////////////////////////////////// ##
## // TESTS  //////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

TEST_EXTENSIONS = .py .pl
AM_DEFAULT_SOURCE_EXT = .cpp


#
# This is the list of tests to be compiled and executed
#
C_TESTS = \
        buildtest \
        MdsExceptionTest \
        MdsDataTest \
        MdsScalarTest_NumericConversion \
        MdsScalarTest_NumericLimits \
        MdsScalarTest_ScalarCast \
        MdsStringTest \
        MdsCompoundTest \
        MdsSignalTest \
        MdsDimensionTest \
        MdsWindowTest \
        MdsFunctionTest \
        MdsConglomTest \
        MdsRangeTest \
        MdsCallTest \
        MdsActionTest \
        MdsApdTest \
        MdsTreeSuppression \
        MdsTreeTest \
        MdsTreeNodeTest \
        MdsEventSuppression \
        MdsEventTest \
        MdsConnectionTest \
        MdsExpressionCompileTest

#    this code must be launched with mdsip server active ..
#    MdsConnectionSuppression

PY_TESTS = 

PL_TESTS =


TESTS = $(C_TESTS) $(PY_TESTS) $(PL_TESTS)



AM_TESTS_ENVIRONMENT = \
      export LD_LIBRARY_PATH=@MAKESHLIBDIR@;
      
LDADD = $(AM_LIBS) testutils/libMdsTestUtils.a



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
               --suppressions=valgrind-stdlib.supp      \
               --suppressions=valgrind-dlerr.supp       \
               --suppressions=valgrind-connection.supp  \
               --suppressions=valgrind-treenode.supp    \
               --dsymutil=yes --leak-check=yes
endif


#
# Files produced by tests that must be purged
#

MOSTLYCLEANFILES = \
                   test_tree_001.characteristics \
                   test_tree_001.datafile \
                   test_tree_001.tree \
                   test_tree_002.characteristics \
                   test_tree_002.datafile \
                   test_tree_002.tree \
                   test_tree2_model.characteristics \
                   test_tree2_model.datafile \
                   test_tree2_model.tree \
                   test_tree_model.characteristics \
                   test_tree_model.datafile \
                   test_tree_model.tree \
                   leaktest_model.characteristics \
                   leaktest_model.datafile \
                   leaktest_model.tree \
                   test-suite.tap \
                   *.out
                   
                   


## ////////////////////////////////////////////////////////////////////////// ##
## // TARGETS  ////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

.PHONY: tests
tests: $(TESTS)

all-local: tests

check_PROGRAMS = $(TESTS)
check_SCRIPTS  = $(PY_TESTS) $(PL_TESTS)


