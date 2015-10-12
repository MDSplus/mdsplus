
SUBDIRS = backends/check .

include ${top_srcdir}/Makefile.inc
include testing.am


AM_CFLAGS = -I${top_srcdir}/include
AM_CXXFLAGS = 
AM_LIBS = 

           
#
# Static Base library
#
noinst_LIBRARIES = libMdsTest.a
libMdsTest_a_SOURCES = base_backend.c

#
# only check backend for now
#
noinst_LIBRARIES += libMdsTestBackend.a
libMdsTestBackend_a_CFLAGS = -Ibackends/check
libMdsTestBackend_a_SOURCES = check_backend.c


#
# Shared library
#
MdsTestShr = @MAKESHLIBDIR@@LIBPRE@MdsTestShr@SHARETYPE@
$(MdsTestShr): backends/check/libCheck.a libMdsTestBackend.a
	$(CXX) $(TARGET_ARCH) $(CXXFLAGS) $(OUTPUT_OPTION) @LINKSHARED@ -Wl,-whole-archive $^ -Wl,-no-whole-archive $(LDFLAGS) $(LINK_MDSIPSHR) $(LIBS)


TEST_SUBDIRS = \
               testing/selftest \
               mdslib/testing \
               mdsobjects/cpp/testing \
               mdsobjects/python/tests

.ONESHELL:
build_tests: $(MdsTestShr)
	list='$(TEST_SUBDIRS)';
	for subdir in $$list; do echo "Making in $$subdir"; \
	 $(am__cd) ${top_srcdir}/$${subdir} && $(MAKE) $(AM_MAKEFLAGS) all tap; \
	 taps="$${taps} $${subdir}/$(TEST_SUITE_TAP)"; \
	done;
	echo " -------- CUT HERE for Jenkins inclusion ------------- ";	
	for i in $${taps}; do \
	 echo "$$i, "; \
	done;
	echo " ----------------------------------------------------- ";

.ONESHELL:
clean_tests: 
	list='$(TEST_SUBDIRS)';
	for subdir in $$list; do echo "Cleaning in $$subdir"; \
	 $(am__cd) ${top_srcdir}/$${subdir} && $(MAKE) $(AM_MAKEFLAGS) clean; \	 
	done;
	
taps: all clean_tests build_tests

all-local: $(MdsTestShr)

clean-local:
	$(RM) $(MdsTestShr)






