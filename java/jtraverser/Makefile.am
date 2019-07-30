EXTRA_DIST = servlet-2_3-fcs-classfiles.zip
CLEANFILES = $(java_DATA)
dist_java_DATA = jTraverser.jar DeviceBeans.jar
JAVAROOT = $(builddir)
javadir = $(exec_prefix)/java/classes
docsdir = $(exec_prefix)/java/classes/docs

# Create the shell script that runs the program
if MINGW
bin_SCRIPTS = traverser.bat
else
bin_SCRIPTS = jTraverser
CLEANFILES += $(bin_SCRIPTS)
EXTRA_DIST += jTraverser.template
$(bin_SCRIPTS): $(srcdir)/jTraverser.template
	cp $< $@
endif

#$(INSTALL) -D $< $@
# Build the class files
CLASSPATH_ENV = CLASSPATH=$(builddir)/../jscope/jScope.jar

java_SRC = $(subst $(srcdir)/,,$(java_JAVA))
java_CLS = $(addprefix $(JAVAROOT)/,$(java_SRC:.java=.class))
java_JAR = $(java_SRC:.java=*.class)

SOURCES = *.java
GIFS = $(wildcard *.gif)

if USE_JARS_DIR

jTraverser.jar : @JARS_DIR@/javatraverser/jTraverser.jar
	cp $< $@

DeviceBeans.jar : @JARS_DIR@/javatraverser/DeviceBeans.jar
	cp $< $@

else

# Build the final jar
java_DATA = jTraverser.jar DeviceBeans.jar
java_JAVA = $(wildcard $(addprefix $(srcdir)/,$(SOURCES) debug/DEBUG.java))

jTraverser.jar: classjava.stamp
	@ echo "Creating JAR: $@ ..."; \
	$(JAR) cf $@ $(java_JAR) $(addprefix -C ${srcdir} ,$(GIFS))

DeviceBeans.jar: jTraverser.jar DeviceBeansManifest.mf
	@ echo "Creating JAR: $@ ..."; \
	cp -f jTraverser.jar $@ && \
	$(JAR) ufm $@ ${srcdir}/DeviceBeansManifest.mf

endif
