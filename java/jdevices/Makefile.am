JAVASOURCE = 6
JAVABOOTCLASSPATH = -bootclasspath $(RT6JAR)
include ../Makefile.inc.am

EXTRA_DIST = *.form

java_srcdir = $(srcdir)/src/main/java
java_resdir = $(srcdir)/src/main/resources
java_DATA = jDevices.jar
CLASSPATH_ENV = CLASSPATH=$(JTRAVERSER_JAR)

if USE_JARS_DIR

jDevices.jar: @JARS_DIR@/java/jdevices/jDevices.jar
	cp $< $@

else

jDevices.jar: classjava.stamp
	@ echo "Creating JAR: $@ ..."; \
	cp -f $(JAVA_RES) $(JAVAROOT)/;\
	$(JAR) cf $@ -C $(JAVAROOT) .

endif

SOURCE = *.java
RESOURCES = *.form
