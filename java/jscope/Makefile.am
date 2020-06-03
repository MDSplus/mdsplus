JAVASOURCE = 8
include ../Makefile.inc.am

java_srcdir = $(srcdir)/src/main/java
java_resdir = $(srcdir)/src/main/resources
dist_java_DATA = jScope.properties
java_DATA = jScope.jar WaveDisplay.jar
MINDTERM = $(srcdir)/MindTerm.jar
EXTRA_DIST = $(MINDTERM)
DEPENDENCIES = $(MINDTERM)

if USE_JARS_DIR

jScope.jar: @JARS_DIR@/java/jscope/jScope.jar
	cp $< $@

WaveDisplay.jar: @JARS_DIR@/java/jscope/WaveDisplay.jar
	cp $< $@

else

jScope.jar: classjava.stamp
	@ echo "Creating JAR: $@ ...";\
	echo "Main-Class: jScope" >> $(MANIFEST) &&\
	echo "Class-Path: WaveDisplay.jar" >> $(MANIFEST) &&\
	rsync -r $(java_resdir)/ $(JAVAROOT) &&\
	$(JAR) cmf $(MANIFEST) $@ -C $(JAVAROOT) jScope.class -C $(JAVAROOT) mds/jscope

WaveDisplay.jar: classjava.stamp
	@ echo "Creating JAR: $@ ...";\
	$(JAR) cf $@ -C $(JAVAROOT) mds/wavedisplay


SCOPE_CLS = $(SCOPE_SRC:.java=*.class)
WAVED_CLS = $(WAVED_SRC:.java=*.class)
endif

jScope.properties: $(java_resdir)/mds/jscope/jScope.properties
	cp $< $@

if MINGW
bin_SCRIPTS += jScope.bat
else
bin_SCRIPTS += jScope
EXTRA_DIST += jScope.template
jScope: $(srcdir)/jScope.template
	cp $< $@
endif

SOURCE =\
 jScope.java\
 mds/wavedisplay/*.java\
 mds/jscope/*.java
RESOURCES =\
 mds/jscope/about.jpg\
 mds/jscope/colors1.tbl\
 mds/jscope/jScope.properties
