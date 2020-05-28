MVN=mvn -Dmaven.repo.local=/workspace/.m2/repository -DsourceDirectory=$(top_srcdir)/java

.PHONY: maven-build maven-tests maven-clean java java_tests

maven-build:
	$(MVN) package -DskipTests

maven-deploy:
	pushd $(top_srcdir)/java &&( find . -name pom.xml -exec /bin/sh -c 'mkdir -p $(dirname $1);cp -f $0 $1' '{}' $(abs_builddir)'{}' ';'; popd)&&\
	$(MVN) versions:set -DgenerateBackupPoms=false -DnewVersion=$(RELEASE_VERSION) -DartifactId=* &&\
	$(MVN) -s /sign_keys/.m2/settings.xml -Dsettings.security=/sign_keys/.m2/settings-security.xml -DskipTests deploy

maven-tests:
	$(TESTS_ENVIRONMENT) $(MVN) test

maven-clean:
	$(MVN) clean

JAVA_DIRS = mdsplus mdsplus-api jtraverser2 jscope mdsobjects jtraverser devicebeans jdevices jdispatcher

.PHONY: $(all_DIRS) $(clean_DIRS)

all_DIRS = $(addprefix all_, $(JAVA_DIRS) $(TEST_DIRS) tests)
$(all_DIRS):
	@ $(MAKE) $(AM_MAKEFLAGS) -C $(@:all_%=%) all
all: $(all_DIRS)

clean_DIRS = $(addprefix clean_, $(JAVA_DIRS) $(TEST_DIRS) tests)
$(clean_DIRS):
	@ $(MAKE) $(AM_MAKEFLAGS) -C $(@:clean_%=%) clean
clean: $(clean_DIRS)


TEST_DIRS = \
	mdsplus-api/tests \
	mdsobjects/tests

.PHONY: $(tests_DIRS) $(clean_tests_DIRS)

tests_DIRS = $(addprefix tests_, $(TEST_DIRS))
$(tests_DIRS):
	@ $(MAKE) $(AM_MAKEFLAGS) -C $(@:tests_%=%) tests
tests: $(tests_DIRS)

clean_tests_DIRS = $(addprefix clean_, $(TEST_DIRS))
$(clean_tests_DIRS):
	@ $(MAKE) $(AM_MAKEFLAGS) -C $(@:clean_%=%) clean
clean_TESTS: $(clean_tests_DIRS)
clean_tests: $(clean_tests_DIRS)
