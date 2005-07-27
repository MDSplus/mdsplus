.SUFFIXES: .class .java
.java.class:
	$(JDK_DIR)\bin\javac.exe $*.java

SOURCES = \
Action.java\
ActionServer.java\
Balancer.java\
InfoServer.java\
jDispatcher.java\
jDispatcherIp.java\
jDispatchMonitor.java\
jServer.java\
KillServer.java\
MdsHelper.java\
MdsIp.java\
MdsMonitor.java\
MdsMonitorEvent.java\
MdsServer.java\
MdsServerEvent.java\
MdsServerListener.java\
MonitorEvent.java\
MonitorListener.java\
NetworkEvent.java\
NetworkListener.java\
Server.java\
ServerEvent.java\
ServerListener.java\
ServerShowDialog.java

all : $(SOURCES)
	$(JDK_DIR)\bin\javac.exe -classpath ..\java\classes\jScope.jar;..\java\classes\jTraverser.jar  *.java
	$(JDK_DIR)\bin\jar.exe -cf ..\java\classes\jDispatcher.jar *.class
