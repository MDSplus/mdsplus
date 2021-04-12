JAVASOURCE = 6
include ../Makefile.inc.am

java_srcdir = $(srcdir)/src/main/java
java_DATA = MdsPlus.jar
DEPENDENCIES =
if USE_JARS_DIR

$(java_DATA): @JARS_DIR@/java/mdsplus/MdsPlus.jar
	cp $< $@

else

$(java_DATA): $(DEPENDENCIES) classjava.stamp
	@ echo "Creating JAR: $@ ..."; \
	$(JAR) cf $@ -C $(JAVAROOT) .

endif

SOURCE = MdsPlus/*.java
