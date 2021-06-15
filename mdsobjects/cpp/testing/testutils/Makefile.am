
include @top_builddir@/Makefile.inc

AM_DEFAULT_SOURCE_EXT = .cpp

# NOTE: please note that also ${top_srcdir} is added to be able to access the
# mdstcpip includes .. needed by mdsipmain.c
#
AM_CFLAGS = $(TARGET_ARCH) $(WARNFLAGS) $(TEST_CFLAGS) -I${top_srcdir}
AM_CXXFLAGS = $(TARGET_ARCH) $(WARNFLAGS) -Wno-deprecated @CXXFLAGS@ -I${top_srcdir}
AM_LDFLAGS = -L@MAKESHLIBDIR@ $(RPATHLINK),@MAKESHLIBDIR@

AM_LIBS = $(LIBS) -lMdsShr -lTreeShr -lTdiShr -lMdsIpShr -lMdsObjectsCppShr
LDADD = $(AM_LIBS)

library_includedir = $(includedir)/mdsplus/testutils
library_include_HEADERS = \
           CommaInit.h \
           FunctionTypes.h \
           Singleton.h \
           testutils.h \
           type_traits.h \
           unique_ptr.h \
           vector_test.h \
           MdsIpInstancer.h
_SOURCES = \
           testutils.cpp \
           mdsipmain.c


#
# libtool version:
#
# noinst_LTLIBRARIES = libMdsTestUtils.la
# libMdsTestUtils_la_SOURCES = ${_SOURCES}
# libMdsTestUtils_la_LIBADD = $(AM_LIBS)
#


#
# Static TestUtils library
#
noinst_LIBRARIES = libMdsTestUtils.a
libMdsTestUtils_a_SOURCES = $(_SOURCES)


#
# This library aims to provide the test symbols for the MdsCall object check
#
libMdsTestDummy.so: testutils.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) $(AM_CXXFLAGS) -shared -o $@ $^

MOSTLYCLEANFILES = libMdsTestDummy.so

all-local: libMdsTestDummy.so
