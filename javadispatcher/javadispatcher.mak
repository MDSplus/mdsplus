.SUFFIXES: .class .java
.java.class:
	%JDK_DIR%\bin\javac.exe $*.java

SOURCES = \
Action.java             MdsHelper.java          MdsServerEvent.java     NetworkListener.java    jDispatcher.java \
ActionServer.java       MdsIp.java              MdsServerListener.java  Server.java             jDispatcherIp.java \
Balancer.java           MdsMonitor.java         MonitorEvent.java       ServerEvent.java \
MdsMonitorEvent.java    MonitorListener.java    ServerListener.java \
InfoServer.java         MdsServer.java          NetworkEvent.java       jDispatchMonitor.java jServer.java

all : $(SOURCES)
	set CLASSPATH=..\java\classes\MDSplus.jar;..\javascope
	%JDK_DIR%\bin\javac.exe *.java
	%JDK_DIR%\bin\jar.exe -uf ..\java\classes\MDSplus.jar *.class
