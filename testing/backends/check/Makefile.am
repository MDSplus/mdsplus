include @top_builddir@/Makefile.inc

LIBS = -L@MAKESHLIBDIR@ @LIBS@ @LIBM@ @LIBDL@ $(LIBSOCKET)

AM_CFLAGS = $(TARGET_ARCH) $(WARNFLAGS)
AM_CXXFLAGS = $(TARGET_ARCH)
AM_LDFLAGS = $(LIBS)


AM_DEFAULT_SOURCE_EXT = .c
_SOURCES = \
        check_error.c \
	check_list.c	\
	check_log.c	\
	check_msg.c	\
	check_pack.c	\
	check_print.c	\
	check_run.c	\
	check_str.c \
	check.c 
	
#\
#	test_backend.c


#
# Static Test library
#
noinst_LIBRARIES = libCheck.a
libCheck_a_SOURCES = $(_SOURCES)
libCheck_a_CFLAGS = $(AM_CFLAGS) $(CFLAGS)



bin_PROGRAMS = build_test
build_test_CFLAGS = $(AM_CFLAGS) $(CFLAGS)
build_test_LDADD = libCheck.a -lm -lpthread
