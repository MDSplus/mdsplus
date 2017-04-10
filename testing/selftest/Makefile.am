include @top_builddir@/Makefile.inc
include ../../testing/testing.am

AM_CFLAGS   = $(TARGET_ARCH) $(WARNFLAGS) $(TEST_CFLAGS)
AM_CXXFLAGS = $(TARGET_ARCH) $(WARNFLAGS) -Wno-deprecated @CXXFLAGS@ $(TEST_CFLAGS)
AM_LDFLAGS  = -L@MAKESHLIBDIR@ $(RPATHLINK),@MAKESHLIBDIR@ 
LDADD = ${LIBS} $(TEST_LIBS)

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
          
          
PY_TESTS = \
	   pyex1.py \
	   py_fail.py

PL_TESTS =

## // LIST OF ALL TESTS ////////////////////////
TESTS = $(C_TESTS) $(PY_TESTS) $(PL_TESTS)

## // LIST OF TESTS EXPECTED TO FAIL ///////////
XFAIL_TESTS = error_test \
	      example4 \
	      example_assert \
	      fork_fail \
	      timeout_fail \
	      py_fail.py

if ENABLE_VALGRIND
XFAIL_TESTS += leak_test
endif


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


# DISABLE PYTHON LEAK DETECTION: This suppression file shall be removed when
# python will finally introduce the patch to detect if valgrind is running ..
# take a look at the file for details. You can also disable this if you are
# using a Python compiled with --without-pymalloc (only for valgrind as it is 5
# times slower to run)
#
VALGRIND_SUPPRESSIONS_FILES_PY = \
	 ${top_srcdir}/conf/valgrind-python-disableleak.supp


## ////////////////////////////////////////////////////////////////////////// ##
## // TARGETS  ////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##


libMdsTestDummy.so: mdstestdummy.cpp
	$(CC) $(CFLAGS) $(AM_CXXFLAGS) -shared -o $@ $^

.ONESHELL:
print:
	@
	echo "--------------------------------------------------------------";
	echo "host = ${host}";
	echo "TESTS_ENVIRONMENT"; \
	 for _i in ${TESTS_ENVIRONMENT}; do echo "    $${_i}"; done
	echo "LOG_COMPILER = ${LOG_COMPILER}";
	echo "LOG_DRIVER   = ${LOG_DRIVER}";
	echo "%PATH% = $(shell ${TESTS_ENVIRONMENT} wine cmd /c 'echo %PATH%' )"
	echo "--------------------------------------------------------------";

	

test_arch:
	@echo " Testing py arch "
	$(TESTS_ENVIRONMENT) $(PYTHON) $(top_srcdir)/testing/testing.py @MAKESHLIBDIR@@LIBPRE@MdsShr@SHARETYPE@ || echo FAIL;
	$(TESTS_ENVIRONMENT) $(PYTHON) $(top_srcdir)/testing/testing.py @MAKESHLIBDIR@@LIBPRE@MdsTestShr@SHARETYPE@ || echo FAIL;




all-local: $(TESTS) libMdsTestDummy.so

clean-local: clean-local-tests

check_PROGRAMS = $(C_TESTS)
check_SCRIPTS  = $(PY_TESTS) $(PL_TESTS)

