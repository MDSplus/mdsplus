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
tests.jar : @JARS_DIR@/java/mdsplus-api/tests/tests.jar
	cp $< $@
else
tests.jar: $(main_JARS) $(JAVAROOT) classjava.stamp
	@ echo "Creating JAR: $@ ...";\
	$(JAR) cf $@ -C $(JAVAROOT) .
endif



MAIN_JARS = mdsplus-api.jar

SOURCE = \
 mds/*.java \
 mds/data/*.java \
 mds/data/descriptor/*.java \
 mds/data/descriptor_r/*.java

TESTS = \
 mds.Mds_Test\
 mds.MdsShr_Test\
 mds.TdiShr_Test\
 mds.TreeShr_Test\
 mds.data.CONST_Test\
 mds.data.TREE_Test\
 mds.data.descriptor.Descriptor_A_Test\
 mds.data.descriptor.Descriptor_CA_Test\
 mds.data.descriptor.Descriptor_S_Test\
 mds.data.descriptor_r.Function_Test

test_path = $(abs_builddir)/trees
hostfile = $(abs_top_srcdir)/testing/multi.hosts
TESTS_ENVIRONMENT += hostfile="$(hostfile)" test_mdsip=1
