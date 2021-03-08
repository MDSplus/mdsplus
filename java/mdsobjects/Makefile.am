JAVASOURCE = 6
DEPENDENCIES = $(srcdir)/src/main/resources/javax.json-1.0.4.jar
include ../Makefile.inc.am

java_srcdir = $(srcdir)/src/main/java
java_resdir = $(srcdir)/src/main/resources
java_DATA = mdsobjects.jar

if USE_JARS_DIR

$(java_DATA): @JARS_DIR@/java/mdsobjects/mdsobjects.jar
	cp $< $@

else

$(java_DATA): $(DEPENDENCIES) classjava.stamp
	$(JAR) xvf $(java_resdir)/javax.json-1.0.4.jar  
	mv org classes/org 
	mv javax classes/javax 
	@ echo "Creating JAR: $@ ..."; \
	rsync -r $(java_resdir)/ $(JAVAROOT) &&\
	$(JAR) cf $@ -C $(JAVAROOT) .

endif

SOURCE =\
 MDSplus/*.java\
 mds/connection/*.java
