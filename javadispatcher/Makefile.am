JAVAROOT = $(builddir)
CLASSES = $(JAVAROOT)/*.class

JSCOPEJAR = $(top_builddir)/javascope/jScope.jar
JTRAVERSERJAR = $(top_builddir)/javatraverser/jTraverser.jar
CLASSPATH_ENV = CLASSPATH=$(JSCOPEJAR):$(JTRAVERSERJAR)

javadir = $(exec_prefix)/java/classes
java_DATA = jDispatcher.jar

if USE_JARS_DIR

jDispatcher.jar : @JARS_DIR@/javadispatcher/jDispatcher.jar
	cp $< $@

else

java_JAVA = Action.java
java_JAVA += ActionServer.java
java_JAVA += Balancer.java
java_JAVA += InfoServer.java
java_JAVA += jDispatcherIp.java
java_JAVA += jDispatcher.java
java_JAVA += jDispatchMonitor.java
java_JAVA += jServer.java
java_JAVA += KillServer.java
java_JAVA += MdsActionsMonitor.java
java_JAVA += MdsHelper.java
java_JAVA += MdsIp.java
java_JAVA += MdsMonitorEvent.java
java_JAVA += MdsMonitor.java
java_JAVA += MdsServerEvent.java
java_JAVA += MdsServer.java
java_JAVA += MdsServerListener.java
java_JAVA += MonitorEvent.java
java_JAVA += MonitorListener.java
java_JAVA += NetworkEvent.java
java_JAVA += NetworkListener.java
java_JAVA += ServerEvent.java
java_JAVA += ServerInfo.java
java_JAVA += Server.java
java_JAVA += ServerListener.java
java_JAVA += ServerShowDialog.java
java_JAVA += ServersInfoPanel.java

dist_java_DATA = jDispatcher.properties
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

