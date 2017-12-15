
include @top_builddir@/Makefile.inc
include ../../testing/testing.am

JAVA_LOG_COMPILER = java org.junit.runner.JUnitCore
EXTRA_DIST = 
CLEANFILES = $(java_DATA) \
	$(java_CLS) \
	$(java_LOGS)

JAVAROOT = $(abs_builddir)/classes

CLASSPATH_ENV = CLASSPATH=$(JTRAVERSER_JAR):$(JAVAROOT):$(JUNIT)

java_SRC = $(addprefix ${srcdir}/,$(TEST_SRC))
java_CLS = $(addprefix $(JAVAROOT)/,$(TEST_SRC:.java=.class))
java_LOGS = javamdslib.log mdsip.log

JUNIT = $(top_srcdir)/testing/junit.jar
JTRAVERSER_JAR = $(abs_builddir)/../jTraverser.jar

$(JAVAROOT):
	mkdir -p $@

javadir = $(exec_prefix)/java/classes
java_JAVA = $(java_SRC)

TEST_SRC = \
        mds/AllTests.java \
        mds/Mds_Test.java \
        mds/MdsShr_Test.java \
        mds/TreeShr_Test.java \
        mds/data/CONST_Test.java \
        mds/data/TREE_Test.java \
        mds/data/descriptor/Descriptor_A_Test.java \
        mds/data/descriptor/Descriptor_S_Test.java \
        mds/data/descriptor_r/Function_Test.java

test_path = $(abs_builddir)/trees
hostfile = $(top_srcdir)/mdsobjects/python/tests/mdsip.hosts

javamdslib.log: $(java_CLS) $(JAVAROOT)
	mkdir $(test_path) && \
	trap "$(RM) -r $(test_path)" EXIT && \
	$(TEST_ENVIRONMENT) $(CLASSPATH_ENV) test_path=$(test_path) hostfile=$(hostfile) test_mdsip=0 $(JAVA_LOG_COMPILER)  mds.AllTests &> javamdslib.log;
mdsip.log: $(test_CLS) $(JAVAROOT)
	mkdir $(test_path) && \
	trap "$(RM) -r $(test_path)" EXIT && \
	$(TEST_ENVIRONMENT) $(CLASSPATH_ENV) test_path=$(test_path) hostfile=$(hostfile) test_mdsip=1 $(JAVA_LOG_COMPILER)  mds.AllTests &> mdsip.log;
log: javamdslib.log mdsip.log
