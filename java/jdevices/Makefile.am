EXTRA_DIST = *.form
JAVAROOT = $(builddir)
CLEANFILES = $(java_CLS) $(java_DATA)
CLASSPATH_ENV = CLASSPATH=$(top_builddir)/java/jtraverser/jTraverser.jar

javadir = $(exec_prefix)/java/classes

java_SRC = $(subst $(srcdir)/,,$(java_JAVA))
java_CLS = $(addprefix $(JAVAROOT)/,$(java_SRC:.java=.class))
java_JAR = $(java_SRC:.java=*.class)

java_DATA = jDevices.jar
java_JAVA = $(wildcard $(srcdir)/*.java)

if USE_JARS_DIR

jDevices.jar: @JARS_DIR@/java/jdevices/jDevices.jar
	cp $< $@

else

jDevices.jar: classjava.stamp
	@ echo "Creating JAR: $@ ..."; \
	$(JAR) cf $@ $(java_JAR)

endif
