JAVASOURCE = 8
include @top_builddir@/Makefile.inc
include ../Makefile.inc.am

java_srcdir = ${srcdir}
java_DATA = taptest.jar

SOURCE = org/junit/runner/TapTestRunner.java

DEPENDENCIES = $(wildcard $(top_srcdir)/java/tests/*.jar)

# override so it will not be installed
install-javaDATA: $(java_DATA)

if USE_JARS_DIR
$(java_DATA): @JARS_DIR@/java/tests/taptest.jar
	cp $< $@
else
$(java_DATA): classjava.stamp
	@ echo "Creating JAR: $@ ...";\
	$(JAR) cf $@ -C $(JAVAROOT) .
endif
