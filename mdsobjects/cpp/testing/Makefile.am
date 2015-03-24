include ${top_srcdir}/Makefile.inc

AM_DEFAULT_SOURCE_EXT = .cpp

AM_CFLAGS = @SRBINCLUDE@
AM_CXXFLAGS = -Wno-deprecated $(CFLAGS) @CXXFLAGS@
AM_LIBS = -L@MAKESHLIBDIR@ -lMdsShr -lTreeShr -lTdiShr -lMdsIpShr -lMdsObjectsCppShr @LIBS@ @SRBLIB@



if HAVE_CHECK
TESTS = \
        buildtest \
        MdsExceptionTest
else
TESTS =
endif


LDADD = $(AM_LIBS)

if USE_VALGRIND
LOG_COMPILER = valgrind
AM_LOG_FLAGS = --quiet --trace-children=yes --error-exitcode=1 \
               --dsymutil=yes --leak-check=yes
endif

check_PROGRAMS = $(TESTS)



