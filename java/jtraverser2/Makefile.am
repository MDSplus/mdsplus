JAVAROOT = $(builddir)
CLEANFILES = $(java_CLS) $(java_DATA) $(bin_SCRIPTS)
CLASSPATH_ENV = CLASSPATH=$(MDSPLUSAPI)

javadir = $(exec_prefix)/java/classes
docsdir = $(exec_prefix)/java/classes/docs

GIFS_SRC = $(wildcard $(addprefix ${srcdir}/src/main/resources/,$(GIF_SRC)))
java_SRC = $(subst $(srcdir)/src/main/java/,,$(java_JAVA))
java_CLS = $(addprefix $(JAVAROOT)/,$(java_SRC:.java=.class))
java_JAR = $(java_SRC:.java=*.class)

dist_java_DATA = jTraverser2.jar
java_DATA = jTraverser2.jar
java_JAVA = $(wildcard $(addprefix $(srcdir)/src/main/java/,$(SOURCE) debug/DEBUG.java))

MDSPLUSAPI=$(top_builddir)/java/mdsplus-api/mdsplus-api.jar

$(JAVAROOT):
	mkdir -p $@

if USE_JARS_DIR

jTraverser2.jar : @JARS_DIR@/java/jtraverser2/jTraverser2.jar
	cp $< $@

else

jTraverser2.jar: classjava.stamp
	mkdir -p $(JAVAROOT)/jtraverser &&\
	cp -f $(GIFS_SRC) $(JAVAROOT)/jtraverser &&\
	cp -f $(MDSPLUSAPI) "$(abs_builddir)/jTraverser2.jar" &&\
	cd $(JAVAROOT) &&\
	echo "Implementation-Version: $(RELEASE_VERSION)">MANIFEST.MF &&\
	echo "Main-Class: jTraverser"                   >>MANIFEST.MF &&\
	$(JAR) umf MANIFEST.MF $@ $(java_JAR: debug/DEBUG.class=) $(GIF_SRC)

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

# list of source files
SOURCE =\
	jtraverser/*.java\
	jtraverser/dialogs/*.java\
	jtraverser/editor/*.java\
	jtraverser/editor/usage/*.java\
	jtraverser/tools/*.java\
	devices/*.java\
	jTraverser.java
GIF_SRC = jtraverser/*.gif
