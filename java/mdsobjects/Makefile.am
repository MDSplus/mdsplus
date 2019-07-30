JAVAROOT = $(builddir)
CLEANFILES = $(java_CLS) $(java_DATA)
CLASSPATH_ENV = CLASSPATH=../jscope/jScope.jar

javadir = $(exec_prefix)/java/classes

java_SRC = $(subst $(srcdir)/,,$(java_JAVA))
java_CLS = $(addprefix $(JAVAROOT)/,$(java_SRC:.java=.class))
java_JAR = $(java_SRC:.java=*.class)

java_DATA = mdsobjects.jar
java_JAVA = $(wildcard $(srcdir)/MDSplus/*.java)

if USE_JARS_DIR

mdsobjects.jar : @JARS_DIR@/java/mdsobjects/mdsobjects.jar
	cp $< $@

else

mdsobjects.jar: classjava.stamp
	@ echo "Creating JAR: $@ ..."; \
	$(JAR) cf $@ $(java_JAR)

endif
