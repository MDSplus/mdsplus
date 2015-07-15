SUBDIRS = testutils .

include ${top_srcdir}/Makefile.inc

AM_DEFAULT_SOURCE_EXT = .cpp

AM_CFLAGS = @SRBINCLUDE@
AM_CXXFLAGS = -Wno-deprecated $(CFLAGS) @CXXFLAGS@
# AM_LIBS = -L@MAKESHLIBDIR@ -lMdsShr -lTreeShr -lTdiShr -lMdsIpShr @MAKESHLIBDIR@@LIBPRE@MdsObjectsCppShr@SHARETYPE@
AM_LIBS = -L@MAKESHLIBDIR@ -lMdsShr -lTreeShr -lTdiShr -lMdsIpShr -lMdsObjectsCppShr
#@LIBS@ @SRBLIB@


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
        MdsTreeTest \
        TestTreeWrite \
        MdsTreeNodeTest \
        MdsEventTest \
        MdsConnectionTest

else
TESTS =
endif

AM_TESTS_ENVIRONMENT = \
      export LD_LIBRARY_PATH=@MAKESHLIBDIR@;
      
LDADD = $(AM_LIBS) testutils/libMdsTestUtils.a

if USE_VALGRIND
LOG_COMPILER = valgrind
AM_LOG_FLAGS = --quiet --trace-children=yes --error-exitcode=1 \
               --suppressions=valgrind-dlerr.supp \
               --dsymutil=yes --leak-check=yes
endif


tests: $(TESTS)

check_PROGRAMS = $(TESTS)



