JAVASOURCE = 8
include ../../Makefile.tests.am

java_srcdir = $(srcdir)/../src/test/java

DEPENDENCIES += $(MDSOBJECTS_JAR)

SOURCE = \
    MDSplus/*.java

TESTS_ENVIRONMENT += java_test_path=$(abs_builddir)

TESTS = \
 MDSplus.MdsDataTest\
 MDSplus.MdsConglomTest\
 MDSplus.MdsConnectionTest\
 MDSplus.MdsDimensionTest\
 MDSplus.MdsEventTest\
 MDSplus.MdsExpressionCompileTest\
 MDSplus.MdsFunctionTest\
 MDSplus.MdsRangeTest\
 MDSplus.MdsSignalTest\
 MDSplus.MdsStringTest\
 MDSplus.MdsTreeNodeTest\
 MDSplus.MdsTreeTest\
 MDSplus.MdsWindowTest
