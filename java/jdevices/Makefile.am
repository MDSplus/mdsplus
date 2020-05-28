JAVASOURCE = 6
JAVABOOTCLASSPATH = -bootclasspath $(RT6JAR)
include ../Makefile.inc.am

EXTRA_DIST = *.form

java_srcdir = $(srcdir)/src/main/java
java_resdir = $(srcdir)/src/main/resources
java_DATA = jDevices.jar
DEPENDENCIES = $(DEVICE_BEANS_JAR) $(JSCOPE_JAR) $(MDSOBJECTS_JAR) 

if USE_JARS_DIR

$(java_DATA): @JARS_DIR@/java/jdevices/jDevices.jar
	cp $< $@

else

$(java_DATA): $(DEPENDENCIES) classjava.stamp
	@ echo "Creating JAR: $@ ..."; \
	cp -f $(JAVA_RES) $(JAVAROOT)/;\
	$(JAR) cf $@ -C $(JAVAROOT) .

endif

SOURCE = *.java
RESOURCES = *.form
