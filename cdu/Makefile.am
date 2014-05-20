lib_LIBRARIES = libCdu.a
libCdu_a_SOURCES = cli_routines.c cli_util.c dasutil_routines1.c dasutil_routines2.c dasutil_routines3.c

bin_PROGRAMS = cdu
cdu_SOURCES = cdu.c cdu_util.c cdu_write.c
cdu_LDADD = libCdu.a -L$(libdir) -lMdsShr

