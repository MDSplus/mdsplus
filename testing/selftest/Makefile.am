
include @top_builddir@/Makefile.inc
include ../../testing/testing.am

AM_CFLAGS = $(TARGET_ARCH) $(TEST_CFLAGS)
AM_CXXFLAGS = $(TARGET_ARCH) -Wno-deprecated @CXXFLAGS@ $(TEST_CFLAGS)
AM_LDFLAGS = -L@MAKESHLIBDIR@ $(RPATHLINK),@MAKESHLIBDIR@ 
LDADD = $(TEST_LIBS)

## ////////////////////////////////////////////////////////////////////////// ##
## // TESTS  //////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

TEST_EXTENSIONS = .py .pl
AM_DEFAULT_SOURCE_EXT = .c


C_TESTS = \
          leak_test \
          test_options \
          example2 \
          example3 \
          example4 \
          example_assert \
          skip_test \
          error_test \
          fork_success \
          fork_fail \
          timeout_success \
          timeout_fail
          
#          example 
          
PY_TESTS = \
           pyex1.py

PL_TESTS =


TESTS = $(C_TESTS) $(PY_TESTS) $(PL_TESTS)

# $(eval $(call ${srcdir}/prova.sh))


bin_PROGRAMS = example3
example3_SOURCES = example3.cpp

bin_PROGRAMS += example_assert
example_assert_LDADD = ${TEST_LIBS} $(builddir)/libMdsTestDummy.so


#
# Files produced by tests that must be purged
#
MOSTLYCLEANFILES = \
                   *.out \
                   libMdsTestDummy.so


## ////////////////////////////////////////////////////////////////////////// ##
## // TARGETS  ////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##


libMdsTestDummy.so: mdstestdummy.cpp
	$(CC) $(CFLAGS) $(AM_CXXFLAGS) -shared -o $@ $^

# .ONESHELL:
print:
	@
	echo "--------------------------------------------------------------";
	echo "host = ${host}";
	echo "TESTS_ENVIRONMENT = ${TESTS_ENVIRONMENT}";
	echo "LOG_COMPILER = ${LOG_COMPILER}";
	echo "LOG_DRIVER   = ${LOG_DRIVER}";
	echo "%PATH% = $(shell ${TESTS_ENVIRONMENT} wine cmd /c 'echo %PATH%' )"
	echo "MAKE = ${MAKE}"
	echo "--------------------------------------------------------------";
	$(tests_config_dump);

wineconsole:
	$(shell ${TESTS_ENVIRONMENT} wineconsole )


all-local: $(TESTS) libMdsTestDummy.so
clean-local: clean-local-tests                    

check_PROGRAMS = $(C_TESTS)
check_SCRIPTS  = $(PY_TESTS) $(PL_TESTS)


