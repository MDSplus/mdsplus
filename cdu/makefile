include $(MAKE_OPTIONS)

OBJFILES =		\
	libCdu.a(cli_routines.o)	\
	libCdu.a(cli_util.o)		\
	libCdu.a(dasutil_routines1.o)	\
	libCdu.a(dasutil_routines2.o)	\
	libCdu.a(dasutil_routines3.o)

all:	libCdu.a  ../bin/cdu

clean:
	find ../bin -name cdu -exec rm {} \;
	rm *.o *.a

libCdu.a:	$(OBJFILES)
		$(RANLIB)

../bin/cdu:	cdu.o cdu_util.o cdu_write.o
	$(CC) $(CFLAGS) -o $@  cdu.o cdu_util.o cdu_write.o libCdu.a \
		-L../shlib  -lMdsShr

.depend:
	touch .depend
