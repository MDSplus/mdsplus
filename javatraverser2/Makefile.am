EXTRA_DIST =
dist_java_DATA = jTraverser2.jar
CLEANFILES = $(java_DATA) $(addprefix $(JAVAROOT)/,$(java_JAR) debug/DEBUG.class)

JAVAROOT = $(abs_builddir)/classes
javadir = $(exec_prefix)/java/classes
docsdir = $(exec_prefix)/java/classes/docs

# Create the shell script that runs the program
if MINGW
bin_SCRIPTS = traverser2.bat $(JAVAROOT)
else
bin_SCRIPTS = jTraverser2 $(JAVAROOT)
CLEANFILES += jTraverser2
EXTRA_DIST += jTraverser2.template
jTraverser2: $(srcdir)/jTraverser2.template
	cp $< $@
endif

#$(INSTALL) -D $< $@
# Build the class files
CLASSPATH_ENV = CLASSPATH=$(JSCH):$(JAVAROOT)

# Build the final jar
TRAV_GIFS = $(addprefix ${srcdir}/java/,$(TRAV_GIF))

java_SRC = $(TRAV_SRC) $(TOOLS_SRC) $(MDS_SRC)
java_CLS = $(addprefix $(JAVAROOT)/,$(java_SRC:.java=.class))
java_JAR = $(java_SRC:.java=.class) $(TRAV_GIF)

JSCH = $(abs_srcdir)/jsch-0.1.54.jar
MANIFEST = $(abs_builddir)/JTMANIFEST.mf

$(JAVAROOT):
	mkdir -p $@

$(JSCH):
	wget -O $(JSCH) http://sourceforge.net/projects/jsch/files/jsch.jar/0.1.54/jsch-0.1.54.jar/download

$(MANIFEST): $(srcdir)/JTMANIFEST.mf
	cp -f $< $@ &&\
	date -u "+Built-Date: %Y-%m-%d %H:%M:%S">>$@


if USE_JARS_DIR

jTraverser2.jar : @JARS_DIR@/javatraverser2/jTraverser2.jar
	cp $< $@

else

java_DATA = jTraverser2.jar
java_JAVA = $(addprefix ${srcdir}/java/,$(java_SRC) debug/DEBUG.java)

# Build the final jar
jTraverser2.jar: $(JSCH) $(MANIFEST)
	mkdir -p $(JAVAROOT)/jtraverser &&\
	cp -f $(TRAV_GIFS) $(JAVAROOT)/jtraverser &&\
	cp -f $(JSCH) "$(abs_builddir)/jTraverser2.jar" &&\
	cd $(JAVAROOT) &&\
	$(JAR) u0fm "$(abs_builddir)/jTraverser2.jar" $(MANIFEST) $(java_JAR) $(TRAV_GIF)

endif

# Giant list of source files follow
TRAV_SRC =\
	jtraverser/*.java\
	jtraverser/dialogs/*.java\
	jtraverser/editor/*.java\
	jtraverser/editor/usage/*.java\
	devices/*.java\
	devices/acq4xx/*.java\
	jTraverser.java

TOOLS_SRC = jtraverser/tools/*.java

MDS_SRC =\
	mds/*.java\
	mds/data/*.java\
	mds/data/descriptor/*.java\
	mds/data/descriptor_a/*.java\
	mds/data/descriptor_apd/*.java\
	mds/data/descriptor_r/*.java\
	mds/data/descriptor_r/function/*.java\
	mds/data/descriptor_s/*.java\
	mds/mdslib/*.java\
	mds/mdsip/*.java

TRAV_GIF =jtraverser/*.gif
