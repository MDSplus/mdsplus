JAVASOURCE = 8
include ../Makefile.inc.am

java_srcdir = $(srcdir)/src/main/java
java_resdir = $(srcdir)/src/main/resources
dist_java_DATA = jScope.properties
java_DATA = jScope.jar
DEPENDENCIES = $(JSCH) $(MDSOBJECTS_JAR)

if USE_JARS_DIR

jScope.jar: @JARS_DIR@/java/jscope/jScope.jar
	cp $< $@

else

jScope.jar: $(DEPENDENCIES) $(MANIFEST) classjava.stamp
	@ echo "Creating JAR: $@ ...";\
	echo "Main-Class: mds.jscope.jScope" >> $(MANIFEST) &&\
	echo "Class-Path: mdsobjects.jar" >> $(MANIFEST) &&\
	rsync -r $(java_resdir)/ $(JAVAROOT) &&\
	cp -f $(JSCH) $@ &&\
	$(JAR) umf $(MANIFEST) $@ -C $(JAVAROOT) .

endif

jScope.properties: $(java_resdir)/mds/jscope/jScope.properties
	cp $< $@

if MINGW
bin_SCRIPTS += jScope.bat
else
bin_SCRIPTS += jScope
EXTRA_DIST = jScope.template
jScope: $(srcdir)/jScope.template
	cp $< $@
endif

SOURCE =\
 mds/connection/*.java\
 mds/jscope/*.java\
 mds/provider/*.java\
 mds/provider/browser/*.java\
 mds/provider/mds/*.java\
 mds/wave/*.java
RESOURCES =\
 mds/jscope/about.jpg\
 mds/jscope/jScope.properties\
 mds/wave/colors1.tbl

