MVN=mvn -Dmaven.repo.local=/workspace/m2-repo -DoutputDirectory=$(abs_builddir)/target -DsourceDirectory=$(abs_srcdir)/src

EXTRA_DIST =
dist_java_DATA = mdsplus-api.jar
CLEANFILES = $(java_DATA) $(addprefix $(JAVAROOT)/,$(java_JAR) debug/DEBUG.class)

JAVAROOT = $(builddir)
javadir = $(exec_prefix)/java/classes
docsdir = $(exec_prefix)/java/classes/docs

#$(INSTALL) -D $< $@
# Build the class files
CLASSPATH_ENV = CLASSPATH=$(JSCH):$(JAVAROOT)


# Build the final jar
java_SRC = $(subst $(srcdir)/src/main/java/,,$(java_JAVA))
java_CLS = $(addprefix $(JAVAROOT)/,$(java_SRC:.java=.class))
java_JAR = $(java_SRC:.java=*.class)

JSCH = $(abs_srcdir)/jsch-0.1.54.jar

$(JAVAROOT):
	mkdir -p $@

$(JSCH):
	wget -O $(JSCH) http://sourceforge.net/projects/jsch/files/jsch.jar/0.1.54/jsch-0.1.54.jar/download

$(MANIFEST):
	date -u "+Built-Date: %Y-%m-%d %H:%M:%S">$@


if USE_JARS_DIR

mdsplus-api.jar : @JARS_DIR@/java/mdsplus-api/mdsplus-api.jar
	cp $< $@

else

java_DATA = mdsplus-api.jar
java_JAVA = $(wildcard $(addprefix $(srcdir)/src/main/java/,$(MDS_SRC) debug/DEBUG.java))

# Build the final jar
mdsplus-api.jar: $(JSCH) $(java_CLS)
	cp -f $(JSCH) "$(abs_builddir)/mdsplus-api.jar" &&\
	pushd $(JAVAROOT) &&\
	($(JAR) u0f "$(abs_builddir)/mdsplus-api.jar" $(java_JAR) ;	popd)

endif

# lists of source files
MDS_SRC =\
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

.PHONY: maven-build maven-tests maven-clean

maven-build:
	cp -f $(abs_srcdir)/pom.xml ./pom.xml &&\
	$(MVN) versions:set -DnewVersion=${RELEASE_VERSION} &&\
	$(MVN) package -DskipTests &&\
	cp target/mdsplus-api-${RELEASE_VERSION}-jar-with-dependencies.jar mdsplus-api.jar

maven-deploy: mdsplus-api.jar
	$(MVN) -s /sign_keys/.m2/settings.xml -Dsettings.security=/sign_keys/.m2/settings-security.xml -DskipTests deploy

maven-tests:
	$(TEST_ENVIRONMENT) $(MVN) test

maven-clean:
	$(MVN) clean
