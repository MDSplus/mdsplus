include ${top_srcdir}/Makefile.inc

AM_DEFAULT_SOURCE_EXT = .cpp

AM_CFLAGS = @SRBINCLUDE@
AM_CXXFLAGS = -Wno-deprecated $(CFLAGS) @CXXFLAGS@
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
           vector_test.h

_SOURCES = \
           testutils.cpp


# sadly libtool is not to be included

# noinst_LTLIBRARIES = libMdsTestUtils.la
# libMdsTestUtils_la_SOURCES = ${_SOURCES}
# libMdsTestUtils_la_LIBADD = $(AM_LIBS)


noinst_LIBRARIES = libMdsTestUtils.a
libMdsTestUtils_a_SOURCES = $(_SOURCES)



libMdsTestUtils.so: $(_SOURCES)
	$(CC) $(CFLAGS) -shared -o $@ $^
	

all-local: libMdsTestUtils.so
