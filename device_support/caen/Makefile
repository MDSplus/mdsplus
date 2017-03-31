ifeq ("$(wildcard /usr/lib/libCAENVME.so)","")
  $(error CAEN VME library not installed)
endif

CC=g++

CFLAGS =  -fPIC  -o2  -Wall -DLINUX -I${MDSPLUS_DIR}/include 
LDFLAGS =  -L${MDSPLUS_DIR}/lib -lCAENVME -lpthread -lMdsObjectsCppShr 


SOURCES=caenInterface.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
SHAREDLIB=libCaenInterface.so
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


