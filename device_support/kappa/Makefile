#Makefile for KAPPA ZELOS camera support in MDSplus device
#NOTE: to enable debug messages add option -DDEBUG in CFLAGS


# 1) install KAPPA ZELOS SDK4 camera driver (http://www.kappa.de/en/service/downloads/software-full-versions)
# 2) DEFINE in .bashrc the SDK library path as follows:
#        export LD_LIBRARY_PATH=/usr/local/kappasdk/lib:$LD_LIBRARY_PATH
# 3) compile and install ../camera_utils
# 4) change DRIVER_INCLUDE and DRIVER_LIB in this makefile according to the driver path installation
# 5) compile and install


DRIVER_INCLUDE=/usr/local/kappasdk/include
DRIVER_LIB=/usr/local/kappasdk/lib

ifeq ("$(wildcard $(DRIVER_LIB))","")
  $(error  KAPPA ZELOS SDK camera driver not installed)
endif

CC=g++
CFLAGS=-o3 -c -fPIC  -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -I$(DRIVER_INCLUDE) -I$(MDSPLUS_DIR)/include -I../camera_utils

LDFLAGS = -shared -lrt -pthread -L$(MDSPLUS_DIR)/lib -lMdsObjectsCppShr -lMdsIpShr -L$(DRIVER_LIB) -lsdk4acquire -lsdk4zelos 
LDFLAGSexe= -L$(MDSPLUS_DIR)/lib -lMdsObjectsCppShr -lMdsIpShr -lcammdsutils -lcamstreamutils -L$(DRIVER_LIB) -lsdk4acquire -lsdk4zelos -lgevtl -L. -lkappazelos -L../flir -lflirutils

SOURCES=kappazelos.cpp main.cpp
OBJ1=kappazelos.o
OBJ2=main.o
OBJECTS=$(SOURCES:.cpp=.o)
SHAREDLIB=libkappazelos.so  
EXE=testkappazelos

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



