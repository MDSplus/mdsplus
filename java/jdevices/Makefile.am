EXTRA_DIST = *.form
CLEANFILES = $(java_DATA)

JAVAROOT = $(builddir)
CLASSPATH_ENV = CLASSPATH=$(top_builddir)/java/jtraverser/jTraverser.jar
javadir = $(exec_prefix)/java/classes

# Giant list of source files follow
SOURCES = $(wildcard *.java)

java_DATA = jDevices.jar

if USE_JARS_DIR

jDevices.jar: @JARS_DIR@/java/jdevices/jDevices.jar
	cp $< $@

else

java_JAVA = $(SOURCES)

# Build the final jar

jDevices.jar: classjava.stamp
	$(JAR) c0f $@ $(java_JAVA:.java=*.class)

endif

