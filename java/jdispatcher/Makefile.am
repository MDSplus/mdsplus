JAVASOURCE = 8
include ../Makefile.inc.am
java_srcdir = $(srcdir)/src/main/java
dist_java_DATA = src/main/resources/mds/jdispatcher/jDispatcher.properties
java_DATA = jDispatcher.jar
DEPENDENCIES = $(MDSOBJECTS_JAR)

if USE_JARS_DIR

$(java_DATA): @JARS_DIR@/java/jdispatcher/jDispatcher.jar
	cp $< $@

else

$(java_DATA): $(DEPENDENCIES) $(MANIFEST) classjava.stamp
	@ echo "Creating JAR: $@ ...";\
	echo "Main-Class: mds.jdispatcher.jDispatchMonitor" >> $(MANIFEST) &&\
	echo "Class-Path: mdsobjects.jar" >> $(MANIFEST) &&\
	$(JAR) cf $@ -C $(JAVAROOT) .

endif

if MINGW
bin_SCRIPTS += jDispatcherIp.bat jDispatchMonitor.bat jServer.bat
else
bin_SCRIPTS += jDispatcherIp jDispatchMonitor jServer
EXTRA_DIST = jDispatcherIp.template jDispatchMonitor.template jServer.template
jDispatcherIp: jDispatcherIp.template
	cp $< $@
jDispatchMonitor: jDispatchMonitor.template
	cp $< $@
jServer: jServer.template
	cp $< $@
endif

SOURCE = mds/jdispatcher/*.java
