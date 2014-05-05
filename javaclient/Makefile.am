JAVAROOT = $(builddir)
javadir = @MAKEJAVADIR@classes

java_JAVA = MdsPlus.java
java_JAVA += MdsPlusEvent.java
java_JAVA += MdsPlusEvents.java
java_JAVA += MdsPlusException.java
java_JAVA += MdsPlusDescriptor.java

java_DATA = MdsPlus.jar
MdsPlus.jar: classjava.stamp
	$(JAR) c0f $@ $(JAVAROOT)/MdsPlus/*

CLEANFILES = $(JAVAROOT)/MdsPlus/*.class
