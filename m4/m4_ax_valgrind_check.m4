# ===========================================================================
#     http://www.gnu.org/software/autoconf-archive/ax_valgrind_check.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_VALGRIND_CHECK()
#
# DESCRIPTION
#
#   Checks whether Valgrind is present and, if so, allows running `make
#   check` under a variety of Valgrind tools to check for memory and
#   threading errors.
#
#   Defines VALGRIND_CHECK_RULES which should be substituted in your
#   Makefile; and $enable_valgrind which can be used in subsequent configure
#   output. ENABLE_VALGRIND is defined and substituted, and corresponds to
#   the value of the --enable-valgrind option, which defaults to being
#   enabled if Valgrind is installed and disabled otherwise.
#
#   If unit tests are written using a shell script and automake's
#   LOG_COMPILER system, the $(VALGRIND) variable can be used within the
#   shell scripts to enable Valgrind, as described here:
#
#     https://www.gnu.org/software/gnulib/manual/html_node/Running-self_002dtests-under-valgrind.html
#
#   Usage example:
#
#   configure.ac:
#
#     AX_VALGRIND_CHECK
#
#   Makefile.am:
#
#     @VALGRIND_CHECK_RULES@
#     VALGRIND_SUPPRESSIONS_FILES = my-project.supp
#     EXTRA_DIST = my-project.supp
#
#   This results in a "tests-valgrind" rule being added to any Makefile.am
#   which includes "@VALGRIND_CHECK_RULES@" (assuming the module has been
#   configured with --enable-valgrind). Running `make tests-valgrind` in
#   that directory will run the module's test suite (`make check`) once for
#   each of the available Valgrind tools (out of memcheck, helgrind, drd and
#   sgcheck), and will output results to valgrind-suite-$toolname.log for each.
#   The target will succeed if there are zero errors and fail otherwise.
#
#   The macro supports running with and without libtool.
#
# LICENSE
#
#   Copyright (c) 2014, 2015 Philip Withnall <philip.withnall@collabora.co.uk>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 4

dnl TODO: add custom valgrind installation and VALGRIND_LIB
AC_DEFUN([AX_VALGRIND_CHECK],[
	m4_pushdef([enable_valgrind],m4_default($1,[ENABLE_VALGRIND]))

	dnl Check for --enable-valgrind
	AC_ARG_ENABLE([valgrind],
	              [AS_HELP_STRING([--enable-valgrind],
		                      [Whether to enable Valgrind on the unit tests])],
	              [enable_valgrind=$enableval],[enable_valgrind="yes"])

	AS_VAR_IF(enable_valgrind,[yes], [
	  # check for valgrind program
	  AC_CHECK_PROG([VALGRIND],[valgrind],[valgrind])
	  AS_IF([test "${VALGRIND}" = ""],[
		AC_MSG_WARN([Could not find valgrind; either install it or reconfigure with --disable-valgrind])
		AS_VAR_SET(enable_valgrind,[no])
	  ])
	  # Enable valgrind only in debug mode (WARNING $enable_debug must be used to selec debug mode)
	  AS_VAR_IF(enable_debug,[yes],[:],[
	    AC_MSG_WARN([Valgrind works only in debug mode, either reconfigure with --enable-debug or --disable-valgrind])
	    AS_VAR_SET(enable_valgrind,[no])
	  ])
	])

	# Looking for Valgrind specific headers
	AS_VAR_IF([enable_valgrind],[yes],
		  AC_CHECK_HEADER([valgrind/valgrind.h],
				  AS_VAR_SET([have_valgrind_h],[yes]),
				  AS_VAR_SET([have_valgrind_h],[no]))
		  AC_CHECK_HEADER([valgrind/memcheck.h],
				  AS_VAR_SET([have_valgrind_memcheck_h],[yes]),
				  AS_VAR_SET([have_valgrind_memcheck_h],[no])))
	AS_VAR_IF([have_valgrind_h],[yes],
		  AC_DEFINE(HAVE_VALGRIND_H, 1,
		   [Define to 1 if you have the <valgrind/valgrind.h> header file.]))
	AS_VAR_IF([have_valgrind_memcheck_h],[yes],
		  AC_DEFINE(HAVE_VALGRIND_MEMCHECK_H, 1,
		   [Define to 1 if you have the <valgrind/memcheck.h> header file.]))

	# AM CONDITIONAL
	AM_CONDITIONAL(enable_valgrind,[test "${enable_valgrind}" = "yes" ])
	AC_SUBST(enable_valgrind)

	AC_ARG_WITH([valgrind-lib],
	              [AS_HELP_STRING([--with-valgrind-lib],
		                      [Set Valgrind lib directory])],
	              [valgrind_dir=$withval])
	AS_VAR_SET_IF([VALGRIND_LIB], AS_VAR_SET([valgrind_dir],[${VALGRIND_LIB}]))
	AC_SUBST([VALGRIND_LIB],[${valgrind_dir}])
	AM_SUBST_NOTMAKE([VALGRIND_LIB])

	AC_MSG_CHECKING([whether to enable Valgrind on the unit tests])
	AC_MSG_RESULT(${enable_valgrind})





	# Check for Valgrind tools we care about.
	m4_define([valgrind_tool_list],[[memcheck], [helgrind], [drd], [exp-sgcheck]])

        # permits use of AM_TESTS_FD_REDIRECT to change suppressions redirections
        m4_pattern_allow(AM_TESTS_FD_REDIRECT)

	AS_IF([test "$VALGRIND" != ""],[
		m4_foreach([vgtool],[valgrind_tool_list],[
			m4_define([vgtooln],AS_TR_SH(vgtool))
			m4_define([ax_cv_var],[ax_cv_valgrind_tool_]vgtooln)
			AC_CACHE_CHECK([for Valgrind tool ]vgtool,ax_cv_var,[
				ax_cv_var=
				AS_IF([`$VALGRIND --tool=vgtool --help >/dev/null 2>&1`],[
					ax_cv_var="vgtool"
				])
			])

			AC_SUBST([VALGRIND_HAVE_TOOL_]vgtooln,[$ax_cv_var])
		])
	])



dnl
dnl  Parsed substitutions
dnl
AS_VAR_READ([VALGRIND_CHECK_RULES_PRE],[
VALGRIND_LIB             ?= ${VALGRIND_LIB}
])

dnl
dnl  Not parsed substitutions
dnl
VALGRIND_CHECK_RULES='
# //////////////////////////////////////////////////////////////////////////// #
# /// Valgrind check  //////////////////////////////////////////////////////// #
# //////////////////////////////////////////////////////////////////////////// #
#
# Optional:
#  - VALGRIND_SUPPRESSIONS_FILES: Space-separated list of Valgrind suppressions
#    files to load. (Default: empty)
#  - VALGRIND_FLAGS: General flags to pass to all Valgrind tools.
#    (Default: --num-callers=30)
#  - VALGRIND_$toolname_FLAGS: Flags to pass to Valgrind $toolname (one of:
#    memcheck, helgrind, drd, sgcheck). (Default: various)


# Optional variables
VALGRIND_FLAGS           ?=
VALGRIND_FLAGS           += --gen-suppressions=all \
                            --num-callers=30 \
                            --trace-children=yes \
                            --child-silent-after-fork=yes \
                            --trace-children-skip-by-arg=*SetMdsplusFileProtection*

VALGRIND_memcheck_FLAGS  ?=
VALGRIND_memcheck_FLAGS  += --leak-check=full --show-reachable=no

VALGRIND_helgrind_FLAGS  ?=
VALGRIND_helgrind_FLAGS  += --history-level=full

VALGRIND_drd_FLAGS       ?=
VALGRIND_drd_FLAGS       +=

VALGRIND_sgcheck_FLAGS   ?=
VALGRIND_sgcheck_FLAGS   +=

VALGRIND_SUPPRESSIONS    ?=
VALGRIND_SUPPRESSIONS    += $(addprefix --suppressions=,$(VALGRIND_SUPPRESSIONS_FILES))

VALGRIND_SUPPRESSIONS_PY ?=
VALGRIND_SUPPRESSIONS_PY += --suppressions=$(top_srcdir)/conf/valgrind-python.supp \
                            $(addprefix --suppressions=,$(VALGRIND_SUPPRESSIONS_FILES_PY))


VALGRIND_TOOLS ?= memcheck helgrind drd sgcheck


valgrind__test_logs1      = $(if ${VALGRIND_TESTS},${VALGRIND_TESTS},${TESTS})
valgrind__test_logs2      = $(valgrind__test_logs1:=.log)
VALGRIND_LOGS            ?= $(foreach ext,$(TEST_EXTENSIONS),$(valgrind__test_logs2:${ext}.log=.log))


# Internal use
valgrind_log_files = $(addprefix valgrind-suite-,$(addsuffix .log,$(VALGRIND_TOOLS))) \
                     $(TEST_LOGS:.log=-valgrind-*.log) \
		     $(TEST_LOGS:.log=-valgrind-*.log.core.*) \
                     $(TEST_LOGS:.log=-valgrind-*.xml) \
                     $(TEST_LOGS:.log=-valgrind-*.supp)
dnl                  $(foreach tst,$(TESTS),$(addprefix $(tst)-valgrind-,$(addsuffix .xml,$(VALGRIND_TOOLS))))

valgrind_supp_files = $(addsuffix -valgrind.supp,$(TESTS))

valgrind_memcheck_flags = --tool=memcheck $(VALGRIND_memcheck_FLAGS)
valgrind_helgrind_flags = --tool=helgrind $(VALGRIND_helgrind_FLAGS)
valgrind_drd_flags = --tool=drd $(VALGRIND_drd_FLAGS)
valgrind_sgcheck_flags = --tool=exp-sgcheck $(VALGRIND_sgcheck_FLAGS)

valgrind_quiet   = $(valgrind_quiet_$(V))
valgrind_quiet_  = $(valgrind_quiet_$(AM_DEFAULT_VERBOSITY))
valgrind_quiet_0 = --quiet

# Support running with and without libtool.
ifneq ($(LIBTOOL),)
valgrind_lt = $(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=execute
else
valgrind_lt =
endif

#
# Valgrind chain add ons
#
VALGRIND_TESTS_ENVIRONMENT ?=
VALGRIND_TESTS_ENVIRONMENT += \
	VALGRIND=$(VALGRIND) \
	$(if ${VALGRIND_LIB},VALGRIND_LIB=${VALGRIND_LIB}) \
	G_SLICE=always-malloc,debug-blocks \
	G_DEBUG=fatal-warnings,fatal-criticals,gc-friendly

VALGRIND_LOG_COMPILER = \
	$(valgrind_lt) \
	$(VALGRIND) --error-exitcode=1 \
	$(VALGRIND_SUPPRESSIONS)  $(VALGRIND_SUPPRESSIONS_PY) \
	$(valgrind_$(VALGRIND_TOOL)_flags) \
	$(VALGRIND_FLAGS)



.PHONY: tests-valgrind tests-valgrind-tool
.PHONY: tests-valgrind-suppressions tests-valgrind-suppressions-tool
ifeq ($(enable_valgrind),yes)

_print_valgrind_hello = \
  echo ""; \
  echo "---------------------------------------------------------------------------"; \
  echo " [vagrind $(VALGRIND_TOOL)] "; \
  echo " active tool options   :  $(valgrind_$(VALGRIND_TOOL)_flags) "; \
  echo " active valgrind flags : $(VALGRIND_FLAGS) "; \
  echo " active suppressions   : "; \
  for supp in $(VALGRIND_SUPPRESSIONS)  $(VALGRIND_SUPPRESSIONS_PY); do \
    echo "        $${supp}"; \
  done

$(VALGRIND_LOGS):

tests-valgrind:
	@ \
	echo "--- VALGRIND TESTS --- enabled tools: $(foreach tool,$(VALGRIND_TOOLS), $(tool))"; \
	$(MAKE) -k $(BUILD_FLAGS) $(AM_MAKEFLAGS) all VALGRIND_BUILD="yes"; \
	$(foreach tool,$(VALGRIND_TOOLS), \
		$(if $(VALGRIND_HAVE_TOOL_$(tool))$(VALGRIND_HAVE_TOOL_exp_$(tool)), \
			$(MAKE) -k $(TEST_FLAGS) $(AM_MAKEFLAGS) tests-valgrind-tool VALGRIND_TOOL=$(tool); \
		) \
	)

tests-valgrind-tool:
	@list="$(VALGRIND_LOGS)";           test -z "$$list" || rm -f $$list
	@list="$(VALGRIND_LOGS:.log=.trs)"; test -z "$$list" || rm -f $$list
	@ $(_print_valgrind_hello);
	@ \
	$(MAKE) -k $(AM_MAKEFLAGS) $(VALGRIND_LOGS) \
	TESTS_ENVIRONMENT="$(VALGRIND_TESTS_ENVIRONMENT) $(TESTS_ENVIRONMENT)" \
	LOG_COMPILER="$(VALGRIND_LOG_COMPILER) -q --log-file=\$$\$$b-valgrind-$(VALGRIND_TOOL)-%p.log --xml=yes --xml-file=\$$\$$b-valgrind-$(VALGRIND_TOOL)-%p.xml $(LOG_COMPILER)" \
	PY_LOG_COMPILER="$(VALGRIND_LOG_COMPILER) -q --log-file=\$$\$$b-valgrind-$(VALGRIND_TOOL)-%p.log --xml=yes --xml-file=\$$\$$b-valgrind-$(VALGRIND_TOOL)-%p.xml $(PY_LOG_COMPILER)" \
	TEST_SUITE_LOG=valgrind-suite-$(VALGRIND_TOOL).log


tests-valgrind-suppressions:
	@ \
	$(MAKE) VALGRIND_BUILD="yes"; \
	$(foreach tool,$(VALGRIND_TOOLS), \
		$(if $(VALGRIND_HAVE_TOOL_$(tool))$(VALGRIND_HAVE_TOOL_exp_$(tool)), \
			$(MAKE) $(AM_MAKEFLAGS) -k tests-valgrind-suppressions-tool VALGRIND_TOOL=$(tool); \
		) \
	)

tests-valgrind-suppressions-tool:
	@list="$(VALGRIND_LOGS)";           test -z "$$list" || rm -f $$list
	@list="$(VALGRIND_LOGS:.log=.trs)"; test -z "$$list" || rm -f $$list
	@ $(_print_valgrind_hello);
	@ \
	$(MAKE) -k $(AM_MAKEFLAGS) $(VALGRIND_LOGS) \
	TESTS_ENVIRONMENT="$(VALGRIND_TESTS_ENVIRONMENT) $(TESTS_ENVIRONMENT)" \
	LOG_COMPILER="$(VALGRIND_LOG_COMPILER) --gen-suppressions=all --log-fd=11 $(LOG_COMPILER)" \
	PY_LOG_COMPILER="$(VALGRIND_LOG_COMPILER) --gen-suppressions=all --log-fd=11 $(PY_LOG_COMPILER)" \
	AM_TESTS_FD_REDIRECT=" 11>&1 | $(AWK) -f $(top_srcdir)/conf/valgrind-parse-suppressions.awk > \$$\$$b-valgrind-$(VALGRIND_TOOL).supp" \
	TEST_SUITE_LOG=valgrind-suite-$(VALGRIND_TOOL).log

else

tests-valgrind tests-valgrind-tool \
tests-valgrind-suppressions tests-valgrind-suppressions-tool:
	@ \
	echo "  ------------------------------------------  "; \
	echo "  Need to reconfigure with --enable-valgrind  "; \
	echo "  ------------------------------------------  ";
endif



DISTCHECK_CONFIGURE_FLAGS ?=
DISTCHECK_CONFIGURE_FLAGS += --disable-valgrind

MOSTLYCLEANFILES ?=
MOSTLYCLEANFILES += $(valgrind_log_files)
'

dnl VALGRIND_CHECK_RULES subsituition 
AC_SUBST([VALGRIND_CHECK_RULES],["${VALGRIND_CHECK_RULES_PRE} ${VALGRIND_CHECK_RULES}"])
m4_ifdef([_AM_SUBST_NOTMAKE], [_AM_SUBST_NOTMAKE([VALGRIND_CHECK_RULES])])

m4_pushdef([enable_valgrind])
])




# internal utils
AC_DEFUN([AS_VAR_READ],[
read -d '' $1 << _as_read_EOF
$2
_as_read_EOF
])



dnl --- TEST CHAIN ---

dnl LOG_DRIVER = $(SHELL) $(top_srcdir)/conf/test-driver
dnl LOG_COMPILE = $(LOG_COMPILER) $(AM_LOG_FLAGS) $(LOG_FLAGS)
dnl PY_LOG_DRIVER = $(SHELL) $(top_srcdir)/conf/test-driver
dnl PY_LOG_COMPILE = $(PY_LOG_COMPILER) $(AM_PY_LOG_FLAGS) $(PY_LOG_FLAGS)

dnl am__test_logs1 = $(TESTS:=.log)
dnl am__test_logs2 = $(am__test_logs1:.log=.log)
dnl TEST_LOGS = $(am__test_logs2:.test.log=.log)
dnl TEST_LOG_DRIVER = $(SHELL) $(top_srcdir)/conf/test-driver
dnl TEST_LOG_COMPILE = $(TEST_LOG_COMPILER) $(AM_TEST_LOG_FLAGS) $(TEST_LOG_FLAGS)
dnl DIST_SUBDIRS = $(SUBDIRS)

dnl buildtest.log: buildtest$(EXEEXT)
dnl        @p='buildtest$(EXEEXT)'; \
dnl        b='buildtest'; \
dnl        $(am__check_pre) $(LOG_DRIVER) --test-name "$$f" \
dnl        --log-file $$b.log --trs-file $$b.trs \
dnl        $(am__common_driver_flags) $(AM_LOG_DRIVER_FLAGS) $(LOG_DRIVER_FLAGS) -- $(LOG_COMPILE) \
dnl        "$$tst" $(AM_TESTS_FD_REDIRECT)

dnl .py.log:
dnl        @p='$<'; \
dnl        $(am__set_b); \
dnl        $(am__check_pre) $(PY_LOG_DRIVER) --test-name "$$f" \
dnl        --log-file $$b.log --trs-file $$b.trs \
dnl        $(am__common_driver_flags) $(AM_PY_LOG_DRIVER_FLAGS) $(PY_LOG_DRIVER_FLAGS) -- $(PY_LOG_COMPILE) \
dnl        "$$tst" $(AM_TESTS_FD_REDIRECT)
