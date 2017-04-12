#Makefile for FLIR support in MDSplus device
#NOTE: to enable debug messages add option -DDEBUG in CFLAGS


# 1) install PLEORA eBUS SDK camera driver (http://www.pleora.com/support-center/documentation-and-downloads)
# 2) DEFINE in .bashrc the SDK library path as follows:
#        source  /opt/pleora/ebus_sdk/RHEL-CentOS-6-x86_64/bin/set_puregev_env
# 3) compile and install ../camera_utils
# 4) change DRIVER_INCLUDE and DRIVER_LIB in this makefile according to the driver path installation
# 5) compile and install


DRIVER_INCLUDE=/opt/pleora/ebus_sdk/RHEL-CentOS-6-x86_64/include
DRIVER_LIB=/opt/pleora/ebus_sdk/RHEL-CentOS-6-x86_64/lib


CC=g++
CFLAGS=-o3 -c -fPIC  -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -I$(MDSPLUS_DIR)/include -I$(DRIVER_INCLUDE) -I../camera_utils

LDFLAGS= -shared -pthread -L$(MDSPLUS_DIR)/lib -lMdsObjectsCppShr -lMdsIpShr -lcammdsutils -lcamstreamutils -L$(DRIVER_LIB) -lPvDevice -lPvStream -L. -lflirutils
LDFLAGSexe= -pthread -L$(MDSPLUS_DIR)/lib -lMdsObjectsCppShr -lMdsIpShr -lcammdsutils -lcamstreamutils -L$(DRIVER_LIB) -lPvDevice -lPvStream -L. -lflirsc65x -lflirutils


SOURCES=FLIRSC65X.cpp main.cpp
OBJ1=FLIRSC65X.o
OBJ2=main.o
OBJECTS=$(SOURCES:.cpp=.o)
SHAREDLIB=libflirsc65x.so 
EXE=testflirsc65x

all: $(SOURCES) $(EXE)
   
$(EXE): $(OBJECTS)
	$(CC) $(OBJ1) -o $(SHAREDLIB) $(LDFLAGS) 
	$(CC) $(OBJ2) -o $(EXE) $(LDFLAGSexe) 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(SHAREDLIB)
	rm -f $(EXE)

install:
	cp -f $(SHAREDLIB) $(MDSPLUS_DIR)/lib
	cp -f libflirutils.so $(MDSPLUS_DIR)/lib 	 

uninstall:
	rm -f $(MDSPLUS_DIR)/lib/$(SHAREDLIB)
	rm -f $(MDSPLUS_DIR)/lib/libflirutils.so

