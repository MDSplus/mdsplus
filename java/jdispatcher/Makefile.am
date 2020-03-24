JAVASOURCE = 6
JAVABOOTCLASSPATH = -bootclasspath $(RT6JAR)
include ../Makefile.inc.am

java_srcdir = $(srcdir)/src/main/java
dist_java_DATA = jDispatcher.properties
java_DATA = jDispatcher.jar
DEPENDENCIES = $(JSCOPE_JAR) $(MDSOBJECTS_JAR) $(JTRAVERSER_JAR)

if USE_JARS_DIR

$(java_DATA): @JARS_DIR@/java/jdispatcher/jDispatcher.jar
	cp $< $@

else

$(java_DATA): $(DEPENDENCIES) classjava.stamp
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
