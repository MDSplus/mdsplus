JAVASOURCE = 8
include ../Makefile.inc.am

java_srcdir = $(srcdir)/src/main/java
java_DATA = mdsplus-api.jar
DEPENDENCIES = $(JSCH)

if USE_JARS_DIR

$(java_DATA): @JARS_DIR@/java/mdsplus-api/mdsplus-api.jar
	cp $< $@

else

$(java_DATA): $(JSCH) $(MANIFEST) classjava.stamp
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

