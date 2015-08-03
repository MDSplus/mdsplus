SUBDIRS = testutils .

include ${top_srcdir}/Makefile.inc

AM_DEFAULT_SOURCE_EXT = .cpp

AM_CFLAGS = @SRBINCLUDE@
AM_CXXFLAGS = -Wno-deprecated $(CFLAGS) @CXXFLAGS@
AM_LIBS = -L@MAKESHLIBDIR@ -lMdsShr -lTreeShr -lTdiShr -lMdsIpShr -lMdsObjectsCppShr


#
# NOTE: Gnu check must be present in the system and must be found by configure
# process. If gnu check framework is not installed of not found the tests are
# left undefined.
#
if HAVE_CHECK
TESTS = \
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

#
# NOTE: all Suppression suffixed test are not propely tests but programs that
# may rise false positives in valgrind memory check so they may be usefull to
# generate suppressions codes.
#

else
TESTS =
endif

AM_TESTS_ENVIRONMENT = \
      export LD_LIBRARY_PATH=@MAKESHLIBDIR@;
      
LDADD = $(AM_LIBS) testutils/libMdsTestUtils.a



#
# VALGRIND: If valgrind was enabled by configuration entry the execution of
# tests will be performed using the valgrind command below. Insert suppressions
# files there.
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



.PHONY: tests
tests: $(TESTS)

all-local: tests

check_PROGRAMS = $(TESTS)



