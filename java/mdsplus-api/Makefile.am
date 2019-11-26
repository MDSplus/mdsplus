JAVASOURCE = 8
include ../Makefile.inc.am

JSCH = $(abs_srcdir)/jsch-0.1.54.jar

java_srcdir = $(srcdir)/src/main/java
dist_java_DATA = mdsplus-api.jar
java_DATA = mdsplus-api.jar
CLASSPATH_ENV = CLASSPATH=$(JSCH)

$(JSCH):
	wget -O $@ http://sourceforge.net/projects/jsch/files/jsch.jar/0.1.54/jsch-0.1.54.jar/download

if USE_JARS_DIR

mdsplus-api.jar : @JARS_DIR@/java/mdsplus-api/mdsplus-api.jar
	cp $< $@

else

mdsplus-api.jar: $(JSCH) $(MANIFEST) classjava.stamp
	@ echo "Creating JAR: $@ ...";\
	cp -f $(JSCH) $@ &&\
	$(JAR) umf $(MANIFEST) $@ -C $(JAVAROOT) .

endif

SOURCE =\
 mds/*.java\
 mds/data/*.java\
 mds/data/descriptor/*.java\
 mds/data/descriptor_a/*.java\
 mds/data/descriptor_apd/*.java\
 mds/data/descriptor_r/*.java\
 mds/data/descriptor_r/function/*.java\
 mds/data/descriptor_s/*.java\
 mds/mdslib/*.java\
 mds/mdsip/*.java

