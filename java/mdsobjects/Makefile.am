JAVASOURCE = 6
JAVABOOTCLASSPATH = -bootclasspath $(RT6JAR)
include ../Makefile.inc.am

java_srcdir = $(srcdir)/src/main/java
java_DATA = mdsobjects.jar
CLASSPATH_ENV = CLASSPATH=$(JSCOPE_JAR)

if USE_JARS_DIR

mdsobjects.jar : @JARS_DIR@/java/mdsobjects/mdsobjects.jar
	cp $< $@

else

mdsobjects.jar: classjava.stamp
	@ echo "Creating JAR: $@ ..."; \
	$(JAR) cf $@ -C $(JAVAROOT) .

endif

SOURCE = MDSplus/*.java
