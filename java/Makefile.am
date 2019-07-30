# common makefile for java packages
MVN=mvn -Dmaven.repo.local=/workspace/m2-repo -DoutputDirectory=$(abs_builddir)/target -DsourceDirectory=$(abs_srcdir)/src

JAVAROOT = $(builddir)
MANIFEST = $(JAVAROOT)/MANIFEST.MF
javadir = $(exec_prefix)/java/classes
docsdir = $(exec_prefix)/java/classes/docs
CLEANFILES = $(java_DATA) $(java_CLS) debug/DEBUG.class $(bin_SCRIPTS)

GIFS_SRC = $(wildcard $(addprefix $(java_resdir)/,$(GIF_SRC)))
java_SRC = $(subst $(java_srcdir)/,,$(java_JAVA))
java_CLS = $(addprefix $(JAVAROOT)/,$(java_SRC:.java=.class))
java_JAR = $(subst .java,*.class,$(subst debug/DEBUG.java,,$(java_SRC)))

java_JAVA = $(wildcard $(addprefix $(java_srcdir)/,$(SOURCE) debug/DEBUG.java))

$(JAVAROOT):
	mkdir -p $@

$(MANIFEST):
	echo "Specification-Versionn: $(RELEASE_VERSION)"	 >$@
	echo "Implementation-Version: $(RELEASE_VERSION)"	>>$@
	echo "Implementation-Vendor-Id: org.mdsplus"		>>$@

.PHONY: maven-build maven-tests maven-clean

maven-build:
	cp -f $(abs_srcdir)/pom.xml ./pom.xml &&\
	$(MVN) versions:set -DnewVersion=${RELEASE_VERSION} &&\
	$(MVN) package -DskipTests

maven-deploy: mdsplus-api.jar
	$(MVN) -s /sign_keys/.m2/settings.xml -Dsettings.security=/sign_keys/.m2/settings-security.xml -DskipTests deploy

maven-tests:
	$(TEST_ENVIRONMENT) $(MVN) test

maven-clean:
	$(MVN) clean
