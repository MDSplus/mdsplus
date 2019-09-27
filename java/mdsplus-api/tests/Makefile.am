JAVASOURCE = 8
include ../../Makefile.tests.am

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

VALGRIND_TESTS = \
 mds.MdsShr_Test

test_path = $(abs_builddir)/trees
hostfile = $(abs_top_srcdir)/testing/multi.hosts
TESTS_ENVIRONMENT += hostfile="$(hostfile)" test_mdsip=1
