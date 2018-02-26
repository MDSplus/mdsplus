SUBDIRS = testutils .

include @top_builddir@/Makefile.inc
include ../../../testing/testing.am

AM_CFLAGS = $(TARGET_ARCH) $(WARNFLAGS) $(TEST_CFLAGS)
AM_CXXFLAGS = $(TARGET_ARCH) $(WARNFLAGS) -fsigned-char -Wno-deprecated @CXXFLAGS@ $(TEST_CFLAGS)
AM_LDFLAGS = -L@MAKESHLIBDIR@ $(RPATHLINK),@MAKESHLIBDIR@
AM_LIBS = $(LIBS) $(TEST_LIBS) testutils/libMdsTestUtils.a
LDADD = $(AM_LIBS) -lMdsShr -lTreeShr -lTdiShr -lMdsIpShr -lMdsObjectsCppShr \
@MINGW_TRUE@	-lws2_32
## ////////////////////////////////////////////////////////////////////////// ##
## // TESTS  //////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

AM_DEFAULT_SOURCE_EXT = .cpp

BUILD_FLAGS = @MAKEFLAG_J@
# no parallel execution as this currently breaks the MdsConnectionTest "Connection local"
TEST_FLAGS = 

#
# This is the list of tests to be compiled and executed
#
TESTS = \
        buildtest \
        MdsExpressionCompileTest \
        MdsExceptionTest \
        MdsDataTest \
        MdsTdiTest \
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
        MdsTreeSegments \
        MdsEventSuppression \
        MdsEventTest \
	MdsConnectionTest

# set VALGRIND_TESTS to select a subset of tests performed under valgrind if
# the variable is unset all TESTS are added.
#
# VALGRIND_TESTS = 

# Valgrind suppressions:
#
# NOTE: all Suppression suffixed test are not propely tests but programs that
# may rise false positives in valgrind memory check so they may be usefull to
# generate suppressions codes.
#

VALGRIND_SUPPRESSIONS_FILES = \
			      $(top_srcdir)/conf/valgrind-mdsplus.supp \
                              $(top_srcdir)/mdsshr/testing/valgrind-mdsshr.supp \
                              $(srcdir)/valgrind-mdsobjects-cpp.supp

#
# Files produced by tests that must be purged
#
MOSTLYCLEANFILES = *.out \
                   testing.hosts \
                   t_*.*

## ////////////////////////////////////////////////////////////////////////// ##
## // TARGETS  ////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##


all-local: $(TESTS)

clean-local: clean-local-tests

check_PROGRAMS = $(TESTS)
check_SCRIPTS  =


