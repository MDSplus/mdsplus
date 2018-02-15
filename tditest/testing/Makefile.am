
include @top_builddir@/Makefile.inc
include ../../testing/testing.am


## ////////////////////////////////////////////////////////////////////////// ##
## // TESTS  //////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

TEST_EXTENSIONS = .tdi
TDI_LOG_COMPILER= $(abs_top_srcdir)/tditest/testing/do_tditests.sh

TEST_FLAGS = @MAKEFLAG_J@

TESTS = \
	test-tcl-nopy.tdi \
	test-tcl.tdi \
	test-tdishr.tdi \
	test-treeshr.tdi

# if VALGRIND_TESTS is defined this list is executed with valgrind

#	threadsUnitTest.py
VALGRIND_TESTS = \
	test-tcl-nopy.tdi \
	test-tcl.tdi \
	test-tdishr.tdi \
	test-treeshr.tdi

# DISABLE PYTHON LEAK DETECTION: This suppression file shall be removed when
# python will finally introduce the patch to detect if valgrind is running ..
# take a look at the file for details. You can also disable this if you are
# using a Python compiled with --without-pymalloc (only for valgrind as it is 5
# times slower to run)
#
#VALGRIND_SUPPRESSIONS_FILES_PY = \
#	$(top_srcdir)/conf/valgrind-mdsplus.supp \
#	${top_srcdir}/conf/valgrind-python.supp \
#	${top_srcdir}/conf/valgrind-python-disableleak.supp




# Skipping child programs that are launched inside python classes
#
VALGRIND_FLAGS = \
                 --trace-children-skip=*/diff

# Files produced by tests that must be purged
#
MOSTLYCLEANFILES = *.log

## ////////////////////////////////////////////////////////////////////////// ##
## // TARGETS  ////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##


#clean-local: clean-local-tests

check_PROGRAMS = 
check_SCRIPTS  = $(TESTS)
