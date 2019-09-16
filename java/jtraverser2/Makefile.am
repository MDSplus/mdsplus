JAVASOURCE = 8
include ../Makefile.inc.am

java_srcdir = $(srcdir)/src/main/java
java_resdir = $(srcdir)/src/main/resources
dist_java_DATA = jTraverser2.jar
java_DATA = jTraverser2.jar
CLASSPATH_ENV = CLASSPATH=$(MDSPLUS_API_JAR)

if USE_JARS_DIR

jTraverser2.jar : @JARS_DIR@/java/jtraverser2/jTraverser2.jar
	cp $< $@

else

jTraverser2.jar: classjava.stamp
	@ echo "Creating JAR: $@ ...";\
	mkdir -p $(JAVAROOT)/jtraverser &&\
	cp -f $(JAVA_RES) $(JAVAROOT)/jtraverser &&\
	echo "Main-Class: jTraverser" > $(MANIFEST) &&\
	cp -f $(MDSPLUS_API_JAR) $@ &&\
	$(JAR) umf $(MANIFEST) $@ -C $(JAVAROOT) .

endif

if MINGW
bin_SCRIPTS += traverser2.bat
else
bin_SCRIPTS += jTraverser2
EXTRA_DIST = jTraverser2.template
jTraverser2: $(srcdir)/jTraverser2.template
	cp $< $@
endif

SOURCE =\
 jtraverser/*.java\
 jtraverser/dialogs/*.java\
 jtraverser/editor/*.java\
 jtraverser/editor/usage/*.java\
 jtraverser/tools/*.java\
 devices/*.java\
 jTraverser.java
RESOURCES = jtraverser/*.gif
