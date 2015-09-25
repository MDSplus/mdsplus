
SUBDIRS = backends/check .

include ${top_srcdir}/Makefile.inc

AM_DEFAULT_SOURCE_EXT = .c

AM_CFLAGS = -I${top_srcdir}/include -Ibackends
AM_CXXFLAGS = 
AM_LIBS = 

#
# NOTE: Gnu check must be present in the system and must be found by configure
# process. If gnu check framework is not installed of not found the tests are
# left undefined.
#
TESTS = \
        example \
        example2



#
# NOTE: all Suppression suffixed test are not propely tests but programs that
# may rise false positives in valgrind memory check so they may be usefull to
# generate suppressions codes.
#



#AM_TESTS_ENVIRONMENT = export LD_LIBRARY_PATH=@MAKESHLIBDIR@      
LDADD = ${top_srcdir}/testing/backends/check/libCheck.a -lm -lrt -lpthread $(AM_LIBS)
# LDADD = libMdsTest.a 





#
# VALGRIND: If valgrind was enabled by configuration entry the execution of
# tests will be performed using the valgrind command below. Insert suppressions
# files there.
#

if USE_VALGRIND
LOG_COMPILER = valgrind
AM_LOG_FLAGS = --quiet --trace-children=yes --error-exitcode=1 \
               --dsymutil=yes --leak-check=yes
endif



bin_PROGRAMS = example3
example3_SOURCES = example3.cpp
           

#
# Static Test library
#
noinst_LIBRARIES = libMdsTest.a
libMdsTest_a_SOURCES = testing.c

#
# Shared library
#
MdsTestShr = @MAKESHLIBDIR@@LIBPRE@MdsTestShr@SHARETYPE@
$(MdsTestShr): backends/check/libCheck.a
	$(CXX) $(TARGET_ARCH) $(CXXFLAGS) $(OUTPUT_OPTION) @LINKSHARED@ -Wl,-whole-archive $^ -Wl,-no-whole-archive $(LDFLAGS) $(LINK_MDSIPSHR) $(LIBS)


.PHONY: tests
tests: $(TESTS) 


all-local: $(MdsTestShr) tests

clean-local:
	$(RM) $(MdsTestShr)



check_PROGRAMS = $(TESTS)




