include $(MAKE_OPTIONS)

CFLAGS =  -g

OBJECTS = \
wfevent.o 

wfevent : ../bin \
../bin/wfevent 

../bin :
	mkdir ../bin

../bin/wfevent : $(OBJECTS)  ../shlib/libipcs.a ../shlib/libMdsShr.a
	$(CC) -Wl,-a archive -o $@ $(OBJECTS)  -L../shlib -lMdsShr -lipcs -lm

clean :
	- rm .depend
	- rm *.o
	- rm ../bin/wfevent

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)
