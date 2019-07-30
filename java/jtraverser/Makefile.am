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

GIFS = $(subst $(srcdir)/,,$(wildcard ${srcdir}/*.gif))
java_DATA = jTraverser.jar DeviceBeans.jar
java_JAVA = $(wildcard $(srcdir)/*.java)

# Build the final jar
if USE_JARS_DIR

jTraverser.jar : @JARS_DIR@/java/jtraverser/jTraverser.jar
	cp $< $@

DeviceBeans.jar : @JARS_DIR@/java/jtraverser/DeviceBeans.jar
	cp $< $@

else

jTraverser.jar: classjava.stamp
	@ echo "Creating JAR: $@ ..."; \
	echo $(JAR) cf $@ $(java_JAR) $(addprefix -C $(srcdir) ,$(GIFS)); \
	$(JAR) cf $@ $(java_JAR) $(addprefix -C $(srcdir) ,$(GIFS))

DeviceBeans.jar: jTraverser.jar DeviceBeansManifest.mf
	@ echo "Creating JAR: $@ ..."; \
	cp -f jTraverser.jar $@ && \
	$(JAR) ufm $@ ${srcdir}/DeviceBeansManifest.mf

endif
