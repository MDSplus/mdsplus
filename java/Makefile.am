# common makefile for java packages
MVN=mvn -Dmaven.repo.local=/workspace/.m2/repository -DsourceDirectory=$(top_srcdir)/java

MDSPLUS_API_JAR = $(top_builddir)/java/mdsplus-api/mdsplus-api.jar
JTRAVERSER_JAR = $(top_builddir)/java/jtraverser/jTraverser.jar
JTRAVERSER2_JAR = $(top_builddir)/java/jtraverser2/jTraverser2.jar
JSCOPE_JAR = $(top_builddir)/java/jscope/jScope.jar

JAVAROOT = $(builddir)
MANIFEST = $(JAVAROOT)/MANIFEST.MF
javadir = $(exec_prefix)/java/classes
docsdir = $(exec_prefix)/java/classes/docs
CLEANFILES = $(java_DATA) $(java_CLS) debug/DEBUG.class $(bin_SCRIPTS)

GIFS_SRC = $(wildcard $(addprefix $(java_resdir)/,$(GIF_SRC)))
java_SRC = $(subst $(java_srcdir)/,,$(java_JAVA))
java_CLS = $(addprefix $(JAVAROOT)/,$(java_SRC:.java=.class))
java_JAR = $(subst .java,*.class,$(subst debug/DEBUG.java,,$(java_SRC)))

if USE_JARS_DIR

java_JAVA =

else

java_JAVA = $(wildcard $(addprefix $(java_srcdir)/,$(SOURCE) debug/DEBUG.java))

endif

$(JAVAROOT):
	mkdir -p $@

$(MANIFEST):
	echo "Specification-Versionn: $(RELEASE_VERSION)"	 >$@
	echo "Implementation-Version: $(RELEASE_VERSION)"	>>$@
	echo "Implementation-Vendor-Id: org.mdsplus"		>>$@

.PHONY: maven-build maven-tests maven-clean

maven-build:
	$(MVN) package -DskipTests


maven-deploy:
	pushd $(top_srcdir)/java &&( find . -name pom.xml -exec /bin/sh -c 'mkdir -p $(dirname $1);cp -f $0 $1' '{}' $(abs_builddir)'{}' ';'; popd)&&\
	$(MVN) versions:set -DgenerateBackupPoms=false -DnewVersion=$(RELEASE_VERSION) -DartifactId=* &&\
	$(MVN) -s /sign_keys/.m2/settings.xml -Dsettings.security=/sign_keys/.m2/settings-security.xml -DskipTests deploy

maven-tests:
	$(TEST_ENVIRONMENT) $(MVN) test

maven-clean:
	$(MVN) clean
