#Makefile for EPIX support in CYGNET MDSplus device
#NOTE: change DRIVER_INCLUDE and DRIVER_LIB for different file location of the EPIX frame grabber library xclib
#NOTE: to enable debug messages add option -DDEBUG in CFLAGS

DRIVER_INCLUDE=/usr/local/xclib
DRIVER_LIB=/usr/local/xclib/xclib_x86_64_pic.a



CC=g++
CFLAGS= -DDEBUG -DC_GNU64=400 -DOS_LINUX  -c -o3 -fPIC  -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -I. -I$(DRIVER_INCLUDE) -I../camera_utils
LDFLAGS= -shared -L../camera_utils -lcammdsutils -lm
OBJ=cygnet.o
SOURCES=cygnet.cpp
SHAREDLIB=libRaptor.so 

all: $(SHAREDLIB)
 

$(OBJ):$(SOURCES)
	$(CC) $(CFLAGS) $< -o $@

$(SHAREDLIB):$(OBJ)
	$(CC) $(OBJ) $(DRIVER_LIB) -o $(SHAREDLIB) $(LDFLAGS) 

clean:
	rm $(OBJ)
	rm $(SHAREDLIB)


