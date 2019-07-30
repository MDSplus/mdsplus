include ../Makefile.am

java_srcdir = $(srcdir)
dist_java_DATA = jDispatcher.properties
java_DATA = jDispatcher.jar
CLASSPATH_ENV = CLASSPATH=$(top_builddir)/java/jscope/jScope.jar:$(top_builddir)/java/jtraverser/jTraverser.jar

if USE_JARS_DIR

jDispatcher.jar : @JARS_DIR@/java/jdispatcher/jDispatcher.jar
	cp $< $@

else

jDispatcher.jar: classjava.stamp
	@ echo "Creating JAR: $@ ..."; \
	$(JAR) cf $@ $(java_JAR)

endif

if MINGW 
bin_SCRIPTS = jDispatcherIp.bat jDispatchMonitor.bat jServer.bat
else
bin_SCRIPTS = jDispatcherIp jDispatchMonitor jServer
EXTRA_DIST = jDispatcherIp.template jDispatchMonitor.template jServer.template
endif

jDispatcherIp jDispatchMonitor jServer: %: %.template
	cp $^ $@

SOURCE = *.java
