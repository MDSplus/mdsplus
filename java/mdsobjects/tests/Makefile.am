JAVASOURCE = 8
include ../../Makefile.tests.am

java_srcdir = ${srcdir}/../src/test/java
java_DATA = tests.jar
main_JARS = $(addprefix ../, $(MAIN_JARS))

null  :=
space := $(null) #
column := :
CLASSPATH_ENV = CLASSPATH=$(subst $(space),$(column),$(strip tests.jar $(main_JARS) $(JUNIT)))

$(main_JARS):
	@ $(MAKE) $(AM_MAKEFLAGS) -C ..

if USE_JARS_DIR
tests.jar : @JARS_DIR@/java/mdsobjects/tests/tests.jar
	cp $< $@
else
tests.jar: $(main_JARS) $(JAVAROOT) classjava.stamp
	@ echo "Creating JAR: $@ ...";\
	$(JAR) cf $@ -C $(JAVAROOT) .
endif

mdsip.sh: $(java_srcdir)/MDSplus/mdsip.sh
	cp $< $@

#MDSplus.MdsConnectionTest.log: mdsip.sh
MOSTLYCLEANFILES = *.tree *.datafile *.characteristics mdsip.sh

tests: mdsip.sh

MAIN_JARS = mdsobjects.jar

SOURCE = \
    MDSplus/*.java

TESTS = \
    MDSplus.MdsDataTest  \
    MDSplus.MdsConglomTest  \
    MDSplus.MdsConnectionTest  \
    MDSplus.MdsDimensionTest  \
    MDSplus.MdsEventTest  \
    MDSplus.MdsExpressionCompileTest  \
    MDSplus.MdsFunctionTest  \
    MDSplus.MdsRangeTest  \
    MDSplus.MdsSignalTest  \
    MDSplus.MdsStringTest  \
    MDSplus.MdsTreeNodeTest  \
    MDSplus.MdsTreeTest  \
    MDSplus.MdsWindowTest 

VALGRIND_TESTS = 

# test_path = $(abs_builddir)/trees
# hostfile = $(abs_top_srcdir)/testing/multi.hosts
# TESTS_ENVIRONMENT += hostfile="$(hostfile)" test_mdsip=1

# these are needed to tell ConnectionTest script how to find mdsip and mdsip.hosts
export MAKEBINDIR
export MAKEETCDIR

# this is needed to run tests uning personalized runner with TAP output
export JUNITTEST = MDSplus.TestRunner
