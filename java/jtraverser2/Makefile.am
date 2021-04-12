JAVASOURCE = 8
include ../Makefile.inc.am

java_srcdir = $(srcdir)/src/main/java
java_resdir = $(srcdir)/src/main/resources
java_DATA = jTraverser2.jar
DEPENDENCIES = $(MDSPLUS_API_JAR)

LAUNCHER = jTraverser2 setupDevice


if USE_JARS_DIR

$(java_DATA): @JARS_DIR@/java/jtraverser2/jTraverser2.jar
	cp $< $@

else

$(java_DATA): $(DEPENDENCIES) $(MANIFEST) classjava.stamp
	@ echo "Creating JAR: $@ ...";\
	echo "Main-Class: mds.jtraverser.jTraverser" >> $(MANIFEST) &&\
	echo "Class-Path: mdsplus-api.jar DeviceBeans.jar WaveDisplay.jar mdsobjects.jar jDevices.jar" >> $(MANIFEST) &&\
	rsync -r $(java_resdir)/ $(JAVAROOT) &&\
	$(JAR) cmf $(MANIFEST) $@ -C $(JAVAROOT) .

endif

if MINGW
bin_SCRIPTS += $(LAUNCHER:=.bat)
else
bin_SCRIPTS += $(LAUNCHER)
EXTRA_DIST = $(LAUNCHER:=.template)
$(LAUNCHER): % : $(srcdir)/%.template
	cp $< $@
endif

SOURCE =\
 mds/jtraverser/*.java\
 mds/jtraverser/dialogs/*.java\
 mds/jtraverser/editor/*.java\
 mds/jtraverser/editor/usage/*.java\
 mds/jtraverser/tools/*.java\
 mds/devices/*.java\
 mds/devices/acq4xx/*.java
RESOURCES = mds/jtraverser/*.gif
