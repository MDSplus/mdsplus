include ../Makefile.am

EXTRA_DIST = *.form

java_srcdir = $(srcdir)
java_DATA = jDevices.jar
CLASSPATH_ENV = CLASSPATH=$(top_builddir)/java/jtraverser/jTraverser.jar

if USE_JARS_DIR

jDevices.jar: @JARS_DIR@/java/jdevices/jDevices.jar
	cp $< $@

else

jDevices.jar: classjava.stamp
	@ echo "Creating JAR: $@ ..."; \
	$(JAR) cf $@ $(java_JAR)

endif

SOURCE = *.java
