include ../Makefile.am

MDSPLUSAPI=$(top_builddir)/java/mdsplus-api/mdsplus-api.jar

java_srcdir = $(srcdir)/src/main/java
java_resdir = $(srcdir)/src/main/resources
dist_java_DATA = jTraverser2.jar
java_DATA = jTraverser2.jar
CLASSPATH_ENV = CLASSPATH=$(MDSPLUSAPI)

if USE_JARS_DIR

jTraverser2.jar : @JARS_DIR@/java/jtraverser2/jTraverser2.jar
	cp $< $@

else

:PHONY $(MANIFEST.MF)

jTraverser2.jar: classjava.stamp $(MANIFEST.MF)
	@ echo "Creating JAR: $@ ...";\
	mkdir -p $(JAVAROOT)/jtraverser &&\
	cp -f $(GIFS_SRC) $(JAVAROOT)/jtraverser &&\
	cp -f $(MDSPLUSAPI) "$(abs_builddir)/jTraverser2.jar" &&\
	echo "Main-Class: jTraverser" >>MANIFEST.MF &&\
	$(JAR) umf MANIFEST.MF $@ $(java_JAR) $(GIF_SRC)

endif

# Create the shell script that runs the program
if MINGW
bin_SCRIPTS = traverser2.bat
else
bin_SCRIPTS = jTraverser2
EXTRA_DIST  = jTraverser2.template
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
GIF_SRC = jtraverser/*.gif
