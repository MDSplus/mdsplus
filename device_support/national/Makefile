ifeq ($(CODAC_ROOT),)
  $(error CODAC_ROOT is not set)
endif

CC=g++

CFLAGS =  -fPIC  -O3 -I${MDSPLUS_DIR}/include -I${CODAC_ROOT}/include 
LDFLAGS = -fPIC  -L${MDSPLUS_DIR}/lib -L${CODAC_ROOT}/lib/  -lpxi6259 -lnixseries -lpthread -lMdsObjectsCppShr 

SOURCES_LIB=NiInterface.cpp 
OBJECTS=$(SOURCES_LIB:.cpp=.o)
SHAREDLIB=libNiInterface.so

SOURCES_EXE=generateWave.cpp probeTermControl.cpp startStopTermControl.cpp  writeDigitalOut.cpp
BIN = $(patsubst %.cpp,%,$(SOURCES_EXE))

all: libNiInterface generateWave probeTermControl startStopTermControl  writeDigitalOut 

libNiInterface : $(OBJECTS)
	$(CC) $(OBJECTS) -o $(SHAREDLIB) -shared $(LDFLAGS) 

.cpp.o:
	$(CC)  -c $(CFLAGS)  $< -o $@

$(BIN):	$(SOURCES_EXE)
	g++ $(LDFLAGS) -L . -lNiInterface $(CFLAGS)  -o $@ $<


clean:
	rm -f $(OBJECTS)
	rm -f $(SHAREDLIB)
	rm -f $(BIN)

install:
	mv  $(SHAREDLIB)  ${MDSPLUS_DIR}/lib

uninstall:
	rm  ${MDSPLUS_DIR}/lib/$(SHAREDLIB)



