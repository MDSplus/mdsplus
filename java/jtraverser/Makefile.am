JAVASOURCE = 6
JAVABOOTCLASSPATH = -bootclasspath $(RT6JAR)
include ../Makefile.inc.am

EXTRA_DIST = servlet-2_3-fcs-classfiles.zip


java_srcdir = $(srcdir)/src/main/java
java_resdir = $(srcdir)/src/main/resources
dist_java_DATA = jTraverser.jar DeviceBeans.jar
java_DATA = jTraverser.jar DeviceBeans.jar
CLASSPATH_ENV = CLASSPATH=$(JSCOPE_JAR)

if USE_JARS_DIR

jTraverser.jar : @JARS_DIR@/java/jtraverser/jTraverser.jar
	cp $< $@

DeviceBeans.jar : @JARS_DIR@/java/jtraverser/DeviceBeans.jar
	cp $< $@

else

jTraverser.jar: classjava.stamp $(srcdir)/DeviceBeansManifest.mf
	@ echo "Creating JAR: $@ ...";\
	cp -f $(JAVA_RES) $(JAVAROOT)/ &&\
	$(JAR) cfm $@ $(srcdir)/DeviceBeansManifest.mf -C $(JAVAROOT) .

DeviceBeans.jar: jTraverser.jar DeviceBeansManifest.mf
	@ echo "Creating JAR: $@ ...";\
	ln -rsfT ./jTraverser.jar $@

endif

if MINGW
bin_SCRIPTS += traverser.bat
else
bin_SCRIPTS += jTraverser
EXTRA_DIST += jTraverser.template
jTraverser: $(srcdir)/jTraverser.template
	cp $< $@
endif

SOURCE = *.java
RESOURCES = *.gif
