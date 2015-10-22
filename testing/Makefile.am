SUBDIRS = backends/check .

include @top_builddir@/Makefile.inc

AM_CFLAGS = -I${top_srcdir}/include $(TARGET_ARCH)
AM_CXXFLAGS = -I${top_srcdir}/include @CXXFLAGS@ $(TARGET_ARCH)
           
#
# Static Base library
#
noinst_LIBRARIES = libMdsTest.a
libMdsTest_a_SOURCES = base_backend.c

#
# only check backend for now
#
noinst_LIBRARIES += libMdsTestBackend.a
libMdsTestBackend_a_CFLAGS = -I${srcdir}/backends/check $(AM_CFLAGS)
libMdsTestBackend_a_SOURCES = check_backend.c


#
# Shared library
#
# @MINGW_TRUE@ DEF=${srcdir}/MdsTestShr.def
@MINGW_TRUE@ MAKE_IMPLIB_MdsTestShr=-Wl,--out-implib,@MAKELIBDIR@MdsTestShr.dll.a
MdsTestShr = @MAKESHLIBDIR@@LIBPRE@MdsTestShr@SHARETYPE@
$(MdsTestShr): backends/check/libCheck.a libMdsTestBackend.a
	$(CXX) $(TARGET_ARCH) $(CXXFLAGS) $(OUTPUT_OPTION) @LINKSHARED@ -Wl,-whole-archive $^ -Wl,-no-whole-archive $(LDFLAGS) $(LIBS) $(MAKE_IMPLIB_MdsTestShr)


TEST_SUBDIRS = \
               mdsobjects/python/tests \
               mdslib/testing \
               mdsobjects/cpp/testing
               
#               testing/selftest


.ONESHELL:
build_tests: $(MdsTestShr)
	list='$(TEST_SUBDIRS)';
	status=0;
	for subdir in $$list; do echo "Making in $$subdir"; \
	 $(am__cd) ${top_builddir}/$${subdir} && $(MAKE) $(AM_MAKEFLAGS) tap || (( status |= 1)); \
	 taps="$${taps} $${subdir}/$(TEST_SUITE_TAP)"; \
	done; \
	echo; \
	echo " COMPONENTS WITH ERRORS FOUND: $${status} "; \
	echo; \
	echo " -------- CUT HERE for Jenkins inclusion ------------- ";	\
	for i in $${taps}; do \
	 echo "$$i"; \
	done; \
	echo " ----------------------------------------------------- ";
	exit $${status}

.ONESHELL:
clean_tests: 
	list='$(TEST_SUBDIRS)';
	for subdir in $$list; do echo "Cleaning in $$subdir"; \
	 $(am__cd) ${top_builddir}/$${subdir} && $(MAKE) $(AM_MAKEFLAGS) clean; \
	done;
	

taps: all clean_tests build_tests

all-local: $(MdsTestShr)

clean-local:
	$(RM) $(MdsTestShr)




