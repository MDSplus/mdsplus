JAVASOURCE = 6
JAVABOOTCLASSPATH = -bootclasspath $(RT6JAR)
include ../Makefile.inc.am

EXTRA_DIST = servlet-2_3-fcs-classfiles.zip


java_srcdir = $(srcdir)/src/main/java
java_resdir = $(srcdir)/src/main/resources
dist_java_DATA = jTraverser.jar DeviceBeans.jar
java_DATA = jTraverser.jar DeviceBeans.jar
DEPENDENCIES = $(JSCOPE_JAR) $(MDSOBJECTS_JAR)

if USE_JARS_DIR

jTraverser.jar: @JARS_DIR@/java/jtraverser/jTraverser.jar
	cp $< $@

else

jTraverser.jar: $(DEPENDENCIES) classjava.stamp $(srcdir)/DeviceBeansManifest.mf
	@ echo "Creating JAR: $@ ...";\
	cp -f $(JAVA_RES) $(JAVAROOT)/ &&\
	$(JAR) cfm $@ $(srcdir)/DeviceBeansManifest.mf -C $(JAVAROOT) .

endif

DeviceBeans.jar: jTraverser.jar
	${LN_R} -sf ./jTraverser.jar $@

if MINGW
bin_SCRIPTS += jTraverser.bat
else
bin_SCRIPTS += jTraverser
EXTRA_DIST += jTraverser.template
jTraverser: $(srcdir)/jTraverser.template
	cp $< $@
endif

SOURCE = *.java
RESOURCES = *.gif
