ifeq ($(CODAC_ROOT),)
  $(error CODAC_ROOT is not set)
endif

CC=g++

#CFLAGS_DEBUG := -fPIC -Werror -Wall -Wextra -O0 -rdynamic -g -DDEBUG
CFLAGS_DEBUG := -fPIC -O0  -g -DDEBUG
LDFLAGS_DEBUG := -Wall -g

# Optimized release builds (default)
# CFLAGS_RELEASE := -fPIC -Werror -Wall -Wextra -O3
CFLAGS_RELEASE := -fPIC  -O3
#LDFLAGS_RELEASE = -Wall
LDFLAGS_RELEASE =

CFLAGS =   -fPIC -O3 -I${MDSPLUS_DIR}/include -I${CODAC_ROOT}/include -std=c++11 
LDFLAGS =  -fPIC -O0 -L${MDSPLUS_DIR}/lib -L${CODAC_ROOT}/lib/ -lm -lNiFpga -lpxi6259 -lnixseries -lpthread -lMdsObjectsCppShr -ltcn -lnisync

# Debug builds (make debug)


SOURCES_LIB=NiInterface.cpp cRioFAUfunction.cpp cRioMPAGfunction.cpp AsyncStoreManager.cpp
OBJECTS=$(SOURCES_LIB:.cpp=.o)
SHAREDLIB=libNiInterface.so
EXE_PROGRAM=generateWave probeTermControl startStopTermControl  writeDigitalOut testELADDigitalOut testELADDigitalIn print_device_info pxi6259devList.sh pxie6368devList.sh

SOURCES_EXE=generateWave.cpp probeTermControl.cpp startStopTermControl.cpp  writeDigitalOut.cpp print_device_info.cpp testELADDigitalOut.cpp testELADDigitalIn.cpp
BIN = $(patsubst %.cpp,%,$(SOURCES_EXE))


all: libNiInterface $(EXE_PROGRAM) ni6683_info


libNiInterface : $(OBJECTS)
	$(CC) $(OBJECTS) -o $(SHAREDLIB) -shared $(LDFLAGS) 

.cpp.o:
	$(CC)  -c $(CFLAGS)  $< -o $@

$(BIN):	$(SOURCES_EXE)
	$(CC) $(LDFLAGS) -L . -lNiInterface $(CFLAGS)  -o $@ $@.cpp

ni6683_info : ni6683_info.c
	$(CC) -L${CODAC_ROOT}/lib -lnisync $(CFLAGS)  -o $@ $@.c

clean:
	rm -f $(OBJECTS)
	rm -f $(SHAREDLIB)
	rm -f $(BIN)

install:
	mv  $(SHAREDLIB)  ${MDSPLUS_DIR}/lib
	cp  $(EXE_PROGRAM) ${MDSPLUS_DIR}/bin
	pxie6368devList.sh $(MDSPLUS_DIR)/tdi/RfxDevices/getPxie6368BoarId.fun
	pxi6259devList.sh $(MDSPLUS_DIR)/tdi/RfxDevices/getPxi6259BoarId.fun
        
updateNIboard:
	pxie6368devList.sh $(MDSPLUS_DIR)/tdi/RfxDevices/getPxie6368BoarId.fun
	pxi6259devList.sh $(MDSPLUS_DIR)/tdi/RfxDevices/getPxi6259BoarId.fun
        

uninstall:
	rm  ${MDSPLUS_DIR}/lib/$(SHAREDLIB)



