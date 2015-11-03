
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


#          example  

C_TESTS = \
          example2 \
          example3 \
          example4 
          
#test_context
          
PY_TESTS = \
           pyex1.py

PL_TESTS =


TESTS = $(C_TESTS) $(PY_TESTS) $(PL_TESTS)

# TESTS_ENVIRONMENT =
# LOG_COMPILER =

# if MINGW
# if HAVE_WINECONSOLE
# TESTS_ENVIRONMENT += WINEPATH='$(shell winepath -w @MAKESHLIBDIR@);$(shell winepath -w $(shell ${CXX} -print-sysroot)/mingw/lib);$(shell winepath -w $(shell ${CXX} -print-sysroot)/mingw/bin)'
# LOG_COMPILER += wine
# else
# TESTS_ENVIRONMENT += @LIBPATH@="@MAKESHLIBDIR@"
# LOG_COMPILER += sh -c 'exit 77'
# endif

# else
# TESTS_ENVIRONMENT += @LIBPATH@="@MAKESHLIBDIR@"
# endif




# $(eval $(call ${srcdir}/prova.sh))



bin_PROGRAMS = example3
example3_SOURCES = example3.cpp


#
# VALGRIND: If valgrind was enabled by configuration entry the execution of
# tests will be performed using the valgrind command below. Insert suppressions
# files there.
#
# NOTE: all Suppression suffixed test are not propely tests but programs that
# may rise false positives in valgrind memory check so they may be usefull to
# generate suppressions codes.
#

#if USE_VALGRIND
#LOG_COMPILER = valgrind
#AM_LOG_FLAGS = --quiet --trace-children=yes --error-exitcode=1 \
#               --dsymutil=yes --leak-check=yes

#endif


#
# Files produced by tests that must be purged
#
MOSTLYCLEANFILES = *.out


## ////////////////////////////////////////////////////////////////////////// ##
## // TARGETS  ////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##


print:
	@echo "--------------------------------------------------------------";
	@echo "host = ${host}";
	@echo "TESTS_ENVIRONMENT = ${TESTS_ENVIRONMENT}";
	@echo "LOG_COMPILER = ${LOG_COMPILER}";
	@echo "LOG_DRIVER   = ${LOG_DRIVER}";
	@echo "%PATH% = $(shell ${TESTS_ENVIRONMENT} wine cmd /c 'echo %PATH%' )"
	@echo "--------------------------------------------------------------";

console:
	$(shell ${TESTS_ENVIRONMENT} wineconsole )


all-local: $(TESTS)
clean-local: clean-local-tests                    

check_PROGRAMS = $(C_TESTS)
check_SCRIPTS  = $(PY_TESTS) $(PL_TESTS)


