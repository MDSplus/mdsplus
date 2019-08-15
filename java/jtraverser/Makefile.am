include ../Makefile.inc.am

EXTRA_DIST = servlet-2_3-fcs-classfiles.zip


java_srcdir = $(srcdir)
java_resdir = $(srcdir)
dist_java_DATA = jTraverser.jar DeviceBeans.jar
java_DATA = jTraverser.jar DeviceBeans.jar
CLASSPATH_ENV = CLASSPATH=$(JSCOPE_JAR)

if USE_JARS_DIR

jTraverser.jar : @JARS_DIR@/java/jtraverser/jTraverser.jar
	cp $< $@

DeviceBeans.jar : @JARS_DIR@/java/jtraverser/DeviceBeans.jar
	cp $< $@

else

jTraverser.jar: classjava.stamp
	@ echo "Creating JAR: $@ ...";\
	cp -f $(GIFS_SRC) $(JAVAROOT)/ &&\
	$(JAR) cf $@ -C $(JAVAROOT) .

DeviceBeans.jar: jTraverser.jar DeviceBeansManifest.mf
	@ echo "Creating JAR: $@ ...";\
	cp -f jTraverser.jar $@ &&\
	$(JAR) ufm $@ ${srcdir}/DeviceBeansManifest.mf

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
GIF_SRC = *.gif
