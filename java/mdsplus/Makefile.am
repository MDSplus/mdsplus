include ../Makefile.am

java_srcdir = $(srcdir)
java_DATA = MdsPlus.jar
CLASSPATH_ENV =

if USE_JARS_DIR

MdsPlus.jar : @JARS_DIR@/java/mdsplus/MdsPlus.jar
	cp $< $@

else

MdsPlus.jar: classjava.stamp
	$(JAR) cf $@ $(java_CLS)

endif

SOURCE = MdsPlus/*.java
