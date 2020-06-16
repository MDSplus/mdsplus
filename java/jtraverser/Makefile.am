JAVASOURCE = 8
include ../Makefile.inc.am

EXTRA_DIST =


java_srcdir = $(srcdir)/src/main/java
java_resdir = $(srcdir)/src/main/resources
java_DATA = jTraverser.jar
DEPENDENCIES = $(MDSOBJECTS_JAR) $(JSCOPE_JAR)

if USE_JARS_DIR

jTraverser.jar: @JARS_DIR@/java/jtraverser/jTraverser.jar
	cp $< $@

else

jTraverser.jar: $(DEPENDENCIES) $(MANIFEST) classjava.stamp
	@ echo "Creating JAR: $@ ...";\
	echo "Main-Class: mds.jtraverser.jTraverser" >> $(MANIFEST) &&\
	echo "Class-Path: mdsobjects.jar DeviceBeans.jar jScope.jar jDevices.jar" >> $(MANIFEST) &&\
	rsync -r $(java_resdir)/ $(JAVAROOT) &&\
	$(JAR) cmf $(MANIFEST) $@ -C $(JAVAROOT) .

endif

if MINGW
bin_SCRIPTS += jTraverser.bat
else
bin_SCRIPTS += jTraverser
bin_SCRIPTS += CompileTree
bin_SCRIPTS += DecompileTree
EXTRA_DIST += jTraverser.template
EXTRA_DIST += CompileTree.template
EXTRA_DIST += DecompileTree.template
jTraverser: $(srcdir)/jTraverser.template
	cp $< $@
CompileTree: $(srcdir)/CompileTree.template
	cp $< $@
DecompileTree: $(srcdir)/DecompileTree.template
	cp $< $@
endif

SOURCE = \
 mds/jtraverser/*.java\
 mds/devices/*.java
RESOURCES = mds/jtraverser/*.gif
