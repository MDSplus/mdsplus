include $(MAKE_OPTIONS)

OBJFILES =		\
	cli_routines.o		\
	cli_util.o		\
	dasutil_routines1.o	\
	dasutil_routines2.o

all:	libCdu.a  cdu

libCdu.a:	libCdu.a( $(OBJFILES) )
		ranlib libCdu.a

cdu:	cdu.o cdu_util.o cdu_write.o
	$(CC) $(CFLAGS) -o $@  $@.o cdu_util.o cdu_write.o libCdu.a

.depend:
	touch .depend
