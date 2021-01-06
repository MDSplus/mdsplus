
include @top_builddir@/Makefile.inc
include ../../../testing/testing.am


## ////////////////////////////////////////////////////////////////////////// ##
## // TESTS  //////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

TEST_EXTENSIONS = .py

TEST_FLAGS = @MAKEFLAG_J@

#	simulateSegfault.py
TESTS = \
	dataUnitTest.py \
	treeUnitTest.py \
	segmentsUnitTest.py \
	taskUnitTest.py \
	connectionUnitTest.py \
	dclUnitTest.py \
	threadsUnitTest.py

VALGRIND_SUPPRESSIONS_FILES =

# Skipping child programs that are launched inside python classes
#
VALGRIND_FLAGS = --trace-children-skip=*/ld,*/collect2,*/ldconfig,*/sh

# Files produced by tests that must be purged
#
MOSTLYCLEANFILES = *.log

## ////////////////////////////////////////////////////////////////////////// ##
## // TARGETS  ////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##


clean-local: clean-local-tests

check_PROGRAMS =
check_SCRIPTS  = $(TESTS)
