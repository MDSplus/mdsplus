
include @top_builddir@/Makefile.inc
include ../../../testing/testing.am


## ////////////////////////////////////////////////////////////////////////// ##
## // TESTS  //////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

TEST_EXTENSIONS = .py



TESTS = \
        test_data.py \
        test_trees.py


# DISABLE PYTHON LEAK DETECTION 
# This suppression file shall be removed when python will finally introduce the
# patch to detect if valgrind is running .. take a look at the file for details.
#
VALGRIND_SUPPRESSIONS_FILES_PY = \
         ${top_srcdir}/conf/valgrind-python-disableleak.supp


#
# Files produced by tests that must be purged
#
MOSTLYCLEANFILES =

## ////////////////////////////////////////////////////////////////////////// ##
## // TARGETS  ////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

clean-local: clean-local-tests

check_PROGRAMS = 
check_SCRIPTS  = $(TESTS)
