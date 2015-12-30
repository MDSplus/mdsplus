SUBDIRS = testutils .

include @top_builddir@/Makefile.inc
include ../../../testing/testing.am

AM_CFLAGS = $(TARGET_ARCH) $(TEST_CFLAGS)
AM_CXXFLAGS = $(TARGET_ARCH) -Wno-deprecated @CXXFLAGS@ $(TEST_CFLAGS)
AM_LDFLAGS = -L@MAKESHLIBDIR@ $(RPATHLINK),@MAKESHLIBDIR@
AM_LIBS = -lMdsShr -lTreeShr -lTdiShr -lMdsIpShr -lMdsObjectsCppShr

LDADD = $(TEST_LIBS) @LIBS@ $(AM_LIBS)

## ////////////////////////////////////////////////////////////////////////// ##
## // TESTS  //////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

AM_DEFAULT_SOURCE_EXT = .cpp


#
# This is the list of tests to be compiled and executed
#
TESTS = \
        buildtest \
        MdsExceptionTest \
        MdsDataTest \
        MdsScalarTest_NumericConversion \
        MdsScalarTest_NumericLimits \
        MdsScalarTest_ScalarCast \
        MdsStringTest \
        MdsCompoundTest \
        MdsSignalTest \
        MdsDimensionTest \
        MdsWindowTest \
        MdsFunctionTest \
        MdsConglomTest \
        MdsRangeTest \
        MdsCallTest \
        MdsActionTest \
        MdsApdTest \
        MdsTreeSuppression \
        MdsTreeTest \
        MdsTreeNodeTest \
        MdsEventSuppression \
        MdsEventTest \
        MdsConnectionTest \
        MdsExpressionCompileTest        

#    this code must be launched with mdsip server active ..
#    MdsConnectionSuppression
      
LDADD += testutils/libMdsTestUtils.a


#
# VALGRIND: If valgrind was enabled by configuration entry the execution of
# tests will be performed using the valgrind command below. Insert suppressions
# files there.
#
# NOTE: all Suppression suffixed test are not propely tests but programs that
# may rise false positives in valgrind memory check so they may be usefull to
# generate suppressions codes.
#

VALGRIND_SUPPRESSIONS_FILES = \
                              $(srcdir)/valgrind-stdlib.supp      \
                              $(srcdir)/valgrind-dlerr.supp       \
                              $(srcdir)/valgrind-connection.supp  \
                              $(srcdir)/valgrind-treenode.supp 



#
# Files produced by tests that must be purged
#
MOSTLYCLEANFILES = *.out \
                   testing.hosts \
                   test_node_001.characteristics \
                   test_node_001.datafile \
                   test_node_001.tree \
                   test_node_002.characteristics \
                   test_node_002.datafile \
                   test_node_002.tree \
                   test_node_model.characteristics \
                   test_node_model.datafile \
                   test_node_model.tree \
                   test_tree_001.characteristics \
                   test_tree_001.datafile \
                   test_tree_001.tree \
                   test_tree_002.characteristics \
                   test_tree_002.datafile \
                   test_tree_002.tree \
                   test_tree2_model.characteristics \
                   test_tree2_model.datafile \
                   test_tree2_model.tree \
                   test_tree_model.characteristics \
                   test_tree_model.datafile \
                   test_tree_model.tree \
                   leaktest_model.characteristics \
                   leaktest_model.datafile \
                   leaktest_model.tree

## ////////////////////////////////////////////////////////////////////////// ##
## // TARGETS  ////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##


all-local: $(TESTS)


SUBDIRS_clean=$(addprefix clean_,$(filter-out .,$(SUBDIRS)))
$(SUBDIRS_clean):
	$(MAKE) -C $(@:clean_%=%) clean

clean-local: clean-local-tests $(SUBDIRS_clean)


check_PROGRAMS = $(TESTS)
check_SCRIPTS  =


