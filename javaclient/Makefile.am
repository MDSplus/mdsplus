JAVAROOT = $(builddir)
CLASSES = $(JAVAROOT)/MdsPlus/*.class

javadir = $(exec_prefix)/java/classes
java_JAVA = MdsPlus.java
java_JAVA += MdsPlusEvent.java
java_JAVA += MdsPlusEvents.java
java_JAVA += MdsPlusException.java
java_JAVA += MdsPlusDescriptor.java

java_DATA = MdsPlus.jar
MdsPlus.jar: classjava.stamp
	$(JAR) c0f $@ $(CLASSES)

CLEANFILES = $(CLASSES) MdsPlus.jar

clean-local:
	-rm -rf $(JAVAROOT)/MdsPlus
