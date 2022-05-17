
CC=g++

CFLAGS =  -fPIC  -std=c++11 -o2  -Wall -DLINUX -I${MDSPLUS_DIR}/include -I.
LDFLAGS =  -L${MDSPLUS_DIR}/lib  -lpthread -lMdsObjectsCppShr 


SOURCES=redpitaya.cpp AsyncStoreManager.cpp redpitaya_dac.cpp
OBJECTS=$(SOURCES:.cpp=.o)
SHAREDLIB=libredpitaya.so
EXE=test

 
all: $(SOURCES) $(EXE) 

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


