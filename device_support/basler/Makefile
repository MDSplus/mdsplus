#Makefile for BASLER cameras support in MDSplus device
#NOTE: to enable debug messages add option -DDEBUG in CFLAGS


# 1) install PYLON SDK camera driver (https://www.baslerweb.com/en/sales-support/downloads/software-downloads/)
# 2) compile and install ../camera_utils
# 3) change DRIVER_INCLUDE and DRIVER_LIB in this makefile according to the driver path installation
# 4) compile and install


PYLON_ROOT ?= /opt/pylon5
DRIVER_INCLUDE += $(shell $(PYLON_ROOT)/bin/pylon-config --cflags)
#LDFLAGS += $(shell $(PYLON_ROOT)/bin/pylon-config --libs-rpath)
DRIVER_LIB += $(shell $(PYLON_ROOT)/bin/pylon-config --libs)
DRIVER_LIB += $(shell $(PYLON_ROOT)/bin/pylon-config --libs-rpath)

#DRIVER_INCLUDE=/opt/pylon5/include
#DRIVER_LIB=/opt/pylon5/lib64


CC=g++
CFLAGS=-O3 -std=c++11 -c -fPIC  -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -I$(MDSPLUS_DIR)/include $(DRIVER_INCLUDE) -I../camera_utils

LDFLAGS= -shared -pthread -L$(MDSPLUS_DIR)/lib -lMdsObjectsCppShr -lMdsIpShr -lcammdsutils -lcamstreamutils $(DRIVER_LIB)
# Search the build directory before MDSPLUS_DIR: an older installed
# libbasleraca.so may have been linked against a different Pylon ABI.
# Use a legacy DT_RPATH for the test executable so that $ORIGIN is searched
# before LD_LIBRARY_PATH, which may contain an older installed libbasleraca.so.
LDFLAGSexe= -pthread -L. -lbasleraca -L$(MDSPLUS_DIR)/lib -lMdsObjectsCppShr -lMdsIpShr -lcammdsutils -lcamstreamutils $(DRIVER_LIB) -Wl,--disable-new-dtags -Wl,-rpath,'$$ORIGIN'


SOURCES=BASLERACA.cpp main.cpp
OBJ1=BASLERACA.o
OBJ2=main.o
OBJECTS=$(SOURCES:.cpp=.o)
SHAREDLIB=libbasleraca.so 
EXE=testbasler

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

uninstall:
	rm -f $(MDSPLUS_DIR)/lib/$(SHAREDLIB)
