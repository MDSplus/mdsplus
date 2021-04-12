JAVASOURCE = 8
include ../Makefile.inc.am

java_srcdir = $(srcdir)/src/main/java
java_resdir = $(srcdir)/src/main/resources
java_DATA = DeviceBeans.jar
DEPENDENCIES = $(MDSOBJECTS_JAR) $(JSCOPE_JAR)

if USE_JARS_DIR

DeviceBeans.jar: @JARS_DIR@/java/devicebeans/DeviceBeans.jar
	cp $< $@

else

DeviceBeans.jar: $(JAVAROOT) $(DEPENDENCIES) classjava.stamp $(srcdir)/DeviceBeansManifest.mf
	@ echo "Creating JAR: $@ ...";\
	cp -f $(JAVA_RES) $(JAVAROOT)/ &&\
	$(JAR) cfm $@ $(srcdir)/DeviceBeansManifest.mf -C $(JAVAROOT) .
endif

SOURCE = *.java\
 mds/devices/*.java
RESOURCES = *.gif
