MVN=mvn -Dmaven.repo.local=/workspace/.m2/repository -DsourceDirectory=$(top_srcdir)/java

.PHONY: maven-build maven-tests maven-clean java java_tests

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


JAVA_DIRS = mdsplus mdsplus-api jscope jtraverser jtraverser2 mdsobjects jdevices jdispatcher

.PHONY: $(all_DIRS) $(clean_DIRS)

all_DIRS = $(addprefix all__, $(JAVA_DIRS))
$(all_DIRS):
	@ $(MAKE) $(AM_MAKEFLAGS) -C $(@:all__%=%) all
all: $(all_DIRS)

clean_DIRS = $(addprefix clean__, $(JAVA_DIRS))
$(clean_DIRS):
	@ $(MAKE) $(AM_MAKEFLAGS) -C $(@:clean__%=%) clean
clean: $(clean_DIRS)


TEST_DIRS = mdsplus-api/tests

.PHONY: $(tests_DIRS) $(clean_tests_DIRS)

tests_DIRS = $(addprefix tests__, $(TEST_DIRS))
$(tests_DIRS):
	@ $(MAKE) $(AM_MAKEFLAGS) -C $(@:tests__%=%) tests
tests: $(tests_DIRS)

clean_tests_DIRS = $(addprefix clean__, $(TEST_DIRS))
$(clean_tests_DIRS):
	@ $(MAKE) $(AM_MAKEFLAGS) -C $(@:tests__%=%) clean
clean_TESTS: $(clean_tests_DIRS)
clean_tests: $(clean_tests_DIRS)
