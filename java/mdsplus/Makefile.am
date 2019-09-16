JAVASOURCE = 6
JAVABOOTCLASSPATH = -bootclasspath $(RT6JAR)
include ../Makefile.inc.am

java_srcdir = $(srcdir)/src/main/java
java_DATA = MdsPlus.jar
CLASSPATH_ENV =

if USE_JARS_DIR

MdsPlus.jar : @JARS_DIR@/java/mdsplus/MdsPlus.jar
	cp $< $@

else

MdsPlus.jar: classjava.stamp
	@ echo "Creating JAR: $@ ..."; \
	$(JAR) cf $@ -C $(JAVAROOT) .

endif

SOURCE = MdsPlus/*.java
