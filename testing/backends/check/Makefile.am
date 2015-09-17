SUBDIRS = lib .

include ${top_srcdir}/Makefile.inc

AM_DEFAULT_SOURCE_EXT = .c

AM_CFLAGS =
AM_CXXFLAGS = 
AM_LIBS = 


_SOURCES = \
        check_error.c \
	check_list.c	\
	check_log.c	\
	check_msg.c	\
	check_pack.c	\
	check_print.c	\
	check_run.c	\
	check_str.c \
	check.c \
	test_backend.c


#
# Static Test library
#
noinst_LIBRARIES = libCheck.a
libCheck_a_SOURCES = $(_SOURCES)



bin_PROGRAMS = build_test
build_test_LDADD = libCheck.a -lm -lpthread
