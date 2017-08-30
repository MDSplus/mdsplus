ifeq ($(CODAC_ROOT),)
  $(error CODAC_ROOT is not set)
endif

CC=gcc

#CFLAGS_DEBUG := -fPIC -Werror -Wall -Wextra -O0 -rdynamic -g -DDEBUG
CFLAGS_DEBUG := -fPIC -O0  -g -DDEBUG
LDFLAGS_DEBUG := -Wall -g

# Optimized release builds (default)
# CFLAGS_RELEASE := -fPIC -Werror -Wall -Wextra -O3
CFLAGS_RELEASE := -fPIC  -O3
#LDFLAGS_RELEASE = -Wall
LDFLAGS_RELEASE =

CFLAGS =   -fPIC -O0 -g -I${MDSPLUS_DIR}/include -I${CODAC_ROOT}/include 
LDFLAGS =  -fPIC -O0 -L${MDSPLUS_DIR}/lib -L${CODAC_ROOT}/lib/ -lNiFpga -lpxi6259 -lnixseries -lpthread -lMdsObjectsCppShr 

# Debug builds (make debug)


SOURCES_LIB=NiInterface.cpp cRioFAUfunction.cpp
OBJECTS=$(SOURCES_LIB:.cpp=.o)
SHAREDLIB=libNiInterface.so
EXE_PROGRAM=generateWave probeTermControl startStopTermControl  writeDigitalOut print_device_info pxi6259devList.sh pxie6368devList.sh

SOURCES_EXE=generateWave.cpp probeTermControl.cpp startStopTermControl.cpp  writeDigitalOut.cpp print_device_info.cpp
BIN = $(patsubst %.cpp,%,$(SOURCES_EXE))


all: libNiInterface $(EXE_PROGRAM) 


libNiInterface : $(OBJECTS)
	$(CC) $(OBJECTS) -o $(SHAREDLIB) -shared $(LDFLAGS) 

.cpp.o:
	$(CC)  -c $(CFLAGS)  $< -o $@

$(BIN):	$(SOURCES_EXE)
	gcc $(LDFLAGS) -L . -lNiInterface $(CFLAGS)  -o $@ $@.cpp 


clean:
	rm -f $(OBJECTS)
	rm -f $(SHAREDLIB)
	rm -f $(BIN)

install:
	mv  $(SHAREDLIB)  ${MDSPLUS_DIR}/lib
	cp  $(EXE_PROGRAM) ${MDSPLUS_DIR}/bin

uninstall:
	rm  ${MDSPLUS_DIR}/lib/$(SHAREDLIB)



