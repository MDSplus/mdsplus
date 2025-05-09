
CC=g++

CFLAGS =  -std=c++11 -fPIC  -o2  -Wall -DLINUX -I${MDSPLUS_DIR}/include -I. -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function
LDFLAGS =  -L${MDSPLUS_DIR}/lib  -lpthread -lMdsObjectsCppShr 


SOURCES=redpitaya.cpp AsyncStoreManager.cpp redpitaya_dac.cpp
OBJECTS=$(SOURCES:.cpp=.o)
SHAREDLIB=libredpitaya.so
EXE=test

all: redpitaya sync_clock
 
sync_clock: sync_clock.c
	$(CC) $(CFLAGS) sync_clock.c -o sync_clock

redpitaya: $(SOURCES) $(EXE) 
	

$(EXE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(SHAREDLIB) -shared $(LDFLAGS) 

.cpp.o:
	$(CC)  -c $(CFLAGS)  $< -o $@

clean:
	rm $(OBJECTS)
	rm $(SHAREDLIB)

install:
	cp $(SHAREDLIB) ${MDSPLUS_DIR}/lib

uninstall:
	rm  ${MDSPLUS_DIR}/lib/$(SHAREDLIB)


