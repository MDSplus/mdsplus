JAVASOURCE = 6
JAVABOOTCLASSPATH = -bootclasspath $(RT6JAR)
include ../Makefile.inc.am

java_srcdir = $(srcdir)/src/main/java
dist_java_DATA = jDispatcher.properties
java_DATA = jDispatcher.jar
CLASSPATH_ENV = CLASSPATH=$(JSCOPE_JAR):$(JTRAVERSER_JAR) 

if USE_JARS_DIR

jDispatcher.jar : @JARS_DIR@/java/jdispatcher/jDispatcher.jar
	cp $< $@

else

jDispatcher.jar: classjava.stamp
	@ echo "Creating JAR: $@ ..."; \
	$(JAR) cf $@ -C $(JAVAROOT) .

endif

if MINGW 
bin_SCRIPTS += jDispatcherIp.bat jDispatchMonitor.bat jServer.bat
else
bin_SCRIPTS += jDispatcherIp jDispatchMonitor jServer
EXTRA_DIST = jDispatcherIp.template jDispatchMonitor.template jServer.template
endif

jDispatcherIp jDispatchMonitor jServer: %: %.template
	cp $^ $@

SOURCE = *.java
