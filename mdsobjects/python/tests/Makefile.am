
include @top_builddir@/Makefile.inc
include ../../../testing/testing.am


## ////////////////////////////////////////////////////////////////////////// ##
## // TESTS  //////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

#PY_LOG_COMPILER=$(if $(PYTHON) $(srcdir)/check_python_arch.py ${MAKESHLIBDIR},@PY_LOG_COMPILER@,${TESTS_SKIP})
#PY_LOG_COMPILER_TAP=$(if $(PYTHON) $(srcdir)/check_python_arch.py ${MAKESHLIBDIR},@PY_LOG_COMPILER_TAP@,${TESTS_SKIP})


TEST_EXTENSIONS = .py



TESTS = \
        dataUnitTest.py \
        treeUnitTest.py \
        threadsUnitTest.py \
        segmentsUnitTest.py
       #simulateSegfault.py 

# if VALGRIND_TESTS is defined this list is executed with valgrind
VALGRIND_TESTS = \
                 dataUnitTest.py \
                 treeUnitTest.py \
                 segmentsUnitTest.py


# DISABLE PYTHON LEAK DETECTION: This suppression file shall be removed when
# python will finally introduce the patch to detect if valgrind is running ..
# take a look at the file for details. You can also disable this if you are
# using a Python compiled with --without-pymalloc (only for valgrind as it is 5
# times slower to run)
#
VALGRIND_SUPPRESSIONS_FILES_PY = \
         ${top_srcdir}/conf/valgrind-python-disableleak.supp



# Skipping child programs that are launched inside python classes
#
VALGRIND_FLAGS = \
                 --trace-children-skip=*/ld \
                 --trace-children-skip=*/collect2 \
                 --trace-children-skip=*/ldconfig \
                 --trace-children-skip=*/sh
                 

# Files produced by tests that must be purged
#
MOSTLYCLEANFILES =

## ////////////////////////////////////////////////////////////////////////// ##
## // TARGETS  ////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##


#nose_test:
#	@ export TEST_FORMAT=${TEST_FORMAT}; \
#	$(TESTS_ENVIRONMENT) $(PYTHON) $(srcdir)/test_data.py

#py_test:
#	$(TESTS_ENVIRONMENT) $(PYTHON) $(top_srcdir)/testing/testing.py \
#	 @MAKESHLIBDIR@@LIBPRE@MdsShr@SHARETYPE@ && \
#	 echo SUCCESS || echo FAIL;


clean-local: clean-local-tests

check_PROGRAMS = 
check_SCRIPTS  = $(TESTS)
