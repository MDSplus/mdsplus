JAVASOURCE = 8
include ../Makefile.inc.am

java_srcdir = $(srcdir)/src/main/java
java_resdir = $(srcdir)/src/main/resources
dist_java_DATA = jTraverser2.jar
java_DATA = jTraverser2.jar
DEPENDENCIES = $(MDSPLUS_API_JAR)

if USE_JARS_DIR

$(java_DATA): @JARS_DIR@/java/jtraverser2/jTraverser2.jar
	cp $< $@

else

$(java_DATA): $(DEPENDENCIES) classjava.stamp
	@ echo "Creating JAR: $@ ...";\
	echo "Main-Class: jTraverser" > $(MANIFEST) &&\
	echo "Class-Path: mdsplus-api.jar DeviceBeans.jar WaveDisplay.jar jDevices.jar" >> $(MANIFEST) &&\
	$(JAR) cmf $(MANIFEST) $@ -C $(JAVAROOT) . -C $(java_resdir) $(RESOURCES)

endif

if MINGW
bin_SCRIPTS += jTraverser2.bat
else
bin_SCRIPTS += jTraverser2
EXTRA_DIST = jTraverser2.template
jTraverser2: $(srcdir)/jTraverser2.template
	cp $< $@
endif

SOURCE =\
 mds/jtraverser/*.java\
 mds/jtraverser/dialogs/*.java\
 mds/jtraverser/editor/*.java\
 mds/jtraverser/editor/usage/*.java\
 mds/jtraverser/tools/*.java\
 mds/devices/*.java\
 mds/devices/acq4xx/*.java\
 jTraverser.java
RESOURCES = mds/jtraverser/*.gif
