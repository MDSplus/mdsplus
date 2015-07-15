include ${top_srcdir}/Makefile.inc

AM_DEFAULT_SOURCE_EXT = .cpp

AM_CFLAGS = @SRBINCLUDE@ -I${top_srcdir}
AM_CXXFLAGS = -Wno-deprecated $(CFLAGS) @CXXFLAGS@ -I${top_srcdir}
AM_LIBS = -L@MAKESHLIBDIR@ -lMdsShr -lTreeShr -lTdiShr -lMdsIpShr @MAKESHLIBDIR@@LIBPRE@MdsObjectsCppShr@SHARETYPE@

# LDADD = $(AM_LIBS)

library_includedir = $(includedir)/mdsplus/testutils
library_include_HEADERS = \
           CommaInit.h \
           FunctionTypes.h \
           Singleton.h \
           testutils.h \
           type_traits.h \
           unique_ptr.h \
           vector_test.h \
           threads.h \
           mdsipmain.h

_SOURCES = \
           testutils.cpp \
           mdsipmain.c


# sadly libtool is not to be included

# noinst_LTLIBRARIES = libMdsTestUtils.la
# libMdsTestUtils_la_SOURCES = ${_SOURCES}
# libMdsTestUtils_la_LIBADD = $(AM_LIBS)


noinst_LIBRARIES = libMdsTestUtils.a
libMdsTestUtils_a_SOURCES = $(_SOURCES)



libMdsTestUtils.so: testutils.cpp
	$(CC) $(CFLAGS) $(AM_CXXFLAGS) -shared -o $@ $^
	

all-local: libMdsTestUtils.so
