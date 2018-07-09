SUBDIRS = backends/check .

include @top_builddir@/Makefile.inc

AM_CFLAGS = -I${top_srcdir}/include $(TARGET_ARCH) $(WARNFLAGS)
AM_CXXFLAGS = -I${top_srcdir}/include @CXXFLAGS@ $(TARGET_ARCH) $(WARNFLAGS)
           
#
# Static Base library
#
noinst_LIBRARIES = libMdsTest.a
libMdsTest_a_CFLAGS = -D_TESTING
libMdsTest_a_SOURCES = base_backend.c

#
# only check backend for now
#
noinst_LIBRARIES += libMdsTestBackend.a
libMdsTestBackend_a_CFLAGS = -D_TESTING -I${srcdir}/backends/check $(AM_CFLAGS)
libMdsTestBackend_a_SOURCES = check_backend.c

#
# Shared library
#
# @MINGW_TRUE@ DEF=${srcdir}/MdsTestShr.def
@MINGW_TRUE@ MAKE_IMPLIB_MdsTestShr=-Wl,--out-implib,@MAKELIBDIR@MdsTestShr.dll.a
MdsTestShr = @MAKESHLIBDIR@@LIBPRE@MdsTestShr@SHARETYPE@
$(MdsTestShr): backends/check/libCheck.a libMdsTestBackend.a
	$(CXX) $(TARGET_ARCH) $(CXXFLAGS) $(OUTPUT_OPTION) @LINKSHARED@ -Wl,-whole-archive $^ -Wl,-no-whole-archive $(LDFLAGS) $(LIBS) $(MAKE_IMPLIB_MdsTestShr)

all-local: $(MdsTestShr)

clean-local:
	-$(RM) $(MdsTestShr)






# //////////////////////////////////////////////////////////////////////////// #
# //  COLLECT TESTS IN SUBDIRECTORIES  /////////////////////////////////////// #
# //////////////////////////////////////////////////////////////////////////// #


TEST_DIRS ?= \
			   mdsobjects/python/tests \
			   mdsshr/testing \
			   mdslib/testing \
			   mdsobjects/cpp/testing \
			   tditest/testing
#               testing/selftest

TEST_OUTDIR  ?= ${top_builddir}/testing

print:
	@echo $(tests_DIRS)
	@echo "$(MAKE) $(AM_MAKEFLAGS)"

.PHONY: $(tests_DIRS) $(clean_DIRS) $(tests_valgrind_DIRS) $(tests_valgrind_suppressions_DIRS)
tests_DIRS  = $(addprefix tests__$(top_builddir)/, $(TEST_DIRS))
rebuild_tests_DIRS  = $(addprefix rebuild_tests__$(top_builddir)/, $(TEST_DIRS))
clean_DIRS  = $(addprefix clean__$(top_builddir)/, $(TEST_DIRS))
tests_valgrind_DIRS = $(addprefix tests_valgrind__$(top_builddir)/, $(TEST_DIRS))
tests_valgrind_suppressions_DIRS = $(addprefix tests_valgrind_suppressions__$(top_builddir)/, $(TEST_DIRS))
$(tests_DIRS):
	@ $(MAKE) $(AM_MAKEFLAGS) -C $(@:tests__%=%) tests

$(rebuild_tests_DIRS):
	@ $(MAKE) $(AM_MAKEFLAGS) -C $(@:rebuild_tests__%=%) rebuild-tests

$(clean_DIRS):
	@-$(MAKE) $(AM_MAKEFLAGS) -C $(@:clean__%=%) clean

$(tests_valgrind_DIRS):	
	@ $(MAKE) $(AM_MAKEFLAGS) -C $(@:tests_valgrind__%=%) tests-valgrind

$(tests_valgrind_suppressions_DIRS):
	@-$(MAKE) $(AM_MAKEFLAGS) -C $(@:tests_valgrind_suppressions__%=%) tests-valgrind-suppressions



.PHONY: tests tests-valgrind tests-valgrind-suppressions tests-clean
tests: $(tests_DIRS)

rebuild-tests: $(rebuild_tests_DIRS)

tests-valgrind: $(tests_valgrind_DIRS)

tests-valgrind-suppressions: $(tests_valgrind_suppressions_DIRS)

tests-clean: $(clean_DIRS)
