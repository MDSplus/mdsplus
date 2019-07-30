JAVAROOT = $(builddir)
CLASSES = $(JAVAROOT)/*.class

JSCOPEJAR = $(top_builddir)/java/jscope/jScope.jar
JTRAVERSERJAR = $(top_builddir)/java/jtraverser/jTraverser.jar
CLASSPATH_ENV = CLASSPATH=$(JSCOPEJAR):$(JTRAVERSERJAR)

javadir = $(exec_prefix)/java/classes
java_DATA = jDispatcher.jar
dist_java_DATA = jDispatcher.properties

if USE_JARS_DIR

jDispatcher.jar : @JARS_DIR@/java/jdispatcher/jDispatcher.jar
	cp $< $@

else

java_JAVA = $(wildcard *.java)

jDispatcher.jar: classjava.stamp
	@ echo "Creating JAR: $@ ..."; \
	  $(JAR) c0f $@ $(CLASSES)

endif

if MINGW 
bin_SCRIPTS = jDispatcherIp.bat jDispatchMonitor.bat jServer.bat
else
bin_SCRIPTS = jDispatcherIp jDispatchMonitor jServer
EXTRA_DIST = jDispatcherIp.template jDispatchMonitor.template jServer.template
endif

CLEANFILES = $(CLASSES) jDispatcher.jar $(bin_SCRIPTS)

jDispatcherIp jDispatchMonitor jServer: %: %.template
	cp $^ $@

