JAVAROOT = $(builddir)
CLASSES = $(JAVAROOT)/MdsPlus/*.class

javadir = $(exec_prefix)/java/classes
java_DATA = MdsPlus.jar

if USE_JARS_DIR

MdsPlus.jar : @JARS_DIR@/javaclient/MdsPlus.jar
	cp $< $@

else

java_JAVA = MdsPlus.java
java_JAVA += MdsPlusEvent.java
java_JAVA += MdsPlusEvents.java
java_JAVA += MdsPlusException.java
java_JAVA += MdsPlusDescriptor.java

MdsPlus.jar: classjava.stamp
	$(JAR) c0f $@ $(CLASSES)

endif

CLEANFILES = $(CLASSES) MdsPlus.jar

clean-local:
	-rm -rf $(JAVAROOT)/MdsPlus
