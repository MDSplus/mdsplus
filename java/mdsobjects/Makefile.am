CLEANFILES = $(java_DATA) MDSplus/*.class
AM_JAVACFLAGS = $(JAVAVERSION)
JAVAROOT = $(builddir)
CLASSPATH_ENV = CLASSPATH=.:$(top_builddir)/java/jscope/jScope.jar
javadir = $(exec_prefix)/java/classes
# Giant list of source files follow

SOURCES = $(wildcard MDSplus/*.java)

# Build the final jar
java_DATA = mdsobjects.jar

if USE_JARS_DIR

mdsobjects.jar : @JARS_DIR@/java/mdsobjects/mdsobjects.jar
	cp $< $@

else

java_JAVA = $(SOURCES)

mdsobjects.jar: classjava.stamp
	$(JAR) c0f $@ $(java_JAVA:.java=*.class)

endif
