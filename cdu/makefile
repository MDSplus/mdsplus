include $(MAKE_OPTIONS)

OBJFILES =		\
	cli_routines.o	\
	cli_util.o		\
	dasutil_routines1.o	\
	dasutil_routines2.o	\
	dasutil_routines3.o

all:	libCdu.a  ../bin/cdu

clean:
	find ../bin -name cdu -exec rm {} \;
	rm *.o *.a

libCdu.a:	$(OBJFILES)
		$(AR) -r libCdu.a $(OBJFILES)
		$(RANLIB)

../bin/cdu:	cdu.o cdu_util.o cdu_write.o
	$(CC) $(CFLAGS) -o $@  cdu.o cdu_util.o cdu_write.o libCdu.a \
		-L../shlib  -lMdsShr -lm

.depend:
	- find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

