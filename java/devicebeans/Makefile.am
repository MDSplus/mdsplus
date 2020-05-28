JAVASOURCE = 6
JAVABOOTCLASSPATH = -bootclasspath $(RT6JAR)
include ../Makefile.inc.am

java_srcdir = $(srcdir)/src/main/java
java_resdir = $(srcdir)/src/main/resources
dist_java_DATA = DeviceBeans.jar
java_DATA = DeviceBeans.jar
DEPENDENCIES = $(JSCOPE_JAR)

if USE_JARS_DIR

DeviceBeans.jar: @JARS_DIR@/java/jtraverser/DeviceBeans.jar 
	cp $< $@

else

DeviceBeans.jar: $(DEPENDENCIES)   classjava.stamp  $(srcdir)/DeviceBeansManifest.mf
	@ echo "Creating JAR: $@ ...";\
	cp -f $(JAVA_RES) $(JAVAROOT)/ &&\
	$(JAR) cfm $@ $(srcdir)/DeviceBeansManifest.mf -C $(JAVAROOT) .
endif

SOURCE = *.java
RESOURCES = *.gif
