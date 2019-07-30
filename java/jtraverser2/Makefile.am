EXTRA_DIST =
dist_java_DATA = jTraverser2.jar
CLEANFILES = $(java_DATA) $(addprefix $(JAVAROOT)/,$(java_JAR) debug/DEBUG.class)

JAVAROOT = $(builddir)
javadir = $(exec_prefix)/java/classes
docsdir = $(exec_prefix)/java/classes/docs

# Create the shell script that runs the program
if MINGW
bin_SCRIPTS = traverser2.bat
else
bin_SCRIPTS = jTraverser2
CLEANFILES += jTraverser2
EXTRA_DIST += jTraverser2.template
jTraverser2: $(srcdir)/jTraverser2.template
	cp $< $@
endif

#$(INSTALL) -D $< $@
# Build the class files
CLASSPATH_ENV = CLASSPATH=$(MDSPLUSAPI):$(JAVAROOT)

# Build the final jar
TRAV_GIFS = $(wildcard $(addprefix ${srcdir}/src/main/resources/,$(TRAV_GIF)))
java_SRC = $(subst $(srcdir)/src/main/java/,,$(java_JAVA))
java_CLS = $(addprefix $(JAVAROOT)/,$(java_SRC:.java=.class))
java_JAR = $(java_SRC:.java=*.class)

MDSPLUSAPI = $(builddir)/../mdsplus-api/mdsplus-api.jar
MANIFEST = $(abs_builddir)/JTMANIFEST.mf

$(JAVAROOT):
	mkdir -p $@

$(MANIFEST): $(srcdir)/JTMANIFEST.mf
	cp -f $< $@ &&\
	date -u "+Built-Date: %Y-%m-%d %H:%M:%S">>$@


if USE_JARS_DIR

jTraverser2.jar : @JARS_DIR@/java/jtraverser2/jTraverser2.jar
	cp $< $@

else

java_DATA = jTraverser2.jar
java_JAVA = $(wildcard $(addprefix $(srcdir)/src/main/java/,$(TRAV_SRC) $(TOOLS_SRC) debug/DEBUG.java))

# Build the final jar
jTraverser2.jar: $(MANIFEST)
	mkdir -p $(JAVAROOT)/jtraverser &&\
	cp -f $(TRAV_GIFS) $(JAVAROOT)/jtraverser &&\
	cp -f $(MDSPLUSAPI) "$(abs_builddir)/jTraverser2.jar" &&\
	cd $(JAVAROOT) &&\
	$(JAR) ufm "$(abs_builddir)/jTraverser2.jar" $(MANIFEST) $(java_JAR) $(TRAV_GIF)

endif

# Giant list of source files follow
TRAV_SRC =\
	jtraverser/*.java\
	jtraverser/dialogs/*.java\
	jtraverser/editor/*.java\
	jtraverser/editor/usage/*.java\
	devices/*.java\
	jTraverser.java

TOOLS_SRC = jtraverser/tools/*.java

TRAV_GIF = jtraverser/*.gif
