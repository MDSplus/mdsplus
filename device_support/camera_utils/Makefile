# Note: Install MDSplus first so that $(MDSPLUS_DIR) exists.

CC=g++
#TO ENABLE DEBUG MESSAGES ADD OPTION -DDEBUG
CFLAGSmdsutils=  -c -o3 -fPIC  -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -I. -I$(MDSPLUS_DIR)/include
CFLAGSstreamutils= -c -o3 -fPIC  -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -I.
##############
LDFLAGSstreamutils = -shared -lrt -pthread 
LDFLAGSmdsutils = -shared -lrt -pthread -L$(MDSPLUS_DIR)/lib -lMdsObjectsCppShr -lMdsIpShr

SOURCESstreamutils=camstreamutils.cpp 
SOURCESmdsutils=cammdsutils.cpp 

INCstreamutils=camstreamutils.h 
INCmdsutils=cammdsutils.h

OBJstreamutils=camstreamutils.o
OBJmdsutils=cammdsutils.o

SHAREDLIBstreamutils=libcamstreamutils.so 
SHAREDLIBmdsutils=libcammdsutils.so 

all: $(SHAREDLIBstreamutils) $(SHAREDLIBmdsutils) 
 
$(OBJstreamutils):$(SOURCESstreamutils)
	$(CC) $(CFLAGSstreamutils) $< -o $@

$(OBJmdsutils):$(SOURCESmdsutils)
	$(CC) $(CFLAGSmdsutils) $< -o $@

$(SHAREDLIBstreamutils):$(OBJstreamutils)
	$(CC) $(OBJstreamutils) -o $(SHAREDLIBstreamutils) $(LDFLAGSstreamutils) 

$(SHAREDLIBmdsutils):$(OBJmdsutils)
	$(CC) $(OBJmdsutils) -o $(SHAREDLIBmdsutils) $(LDFLAGSmdsutils) 

clean:
	rm $(OBJstreamutils)
	rm $(SHAREDLIBstreamutils)
	rm $(OBJmdsutils)
	rm $(SHAREDLIBmdsutils)	

install:
	cp -f $(SHAREDLIBstreamutils) $(MDSPLUS_DIR)/lib
	cp -f $(SHAREDLIBmdsutils) $(MDSPLUS_DIR)/lib 

uninstall:
	rm -f $(MDSPLUS_DIR)/lib/$(SHAREDLIBstreamutils)
	rm -f $(MDSPLUS_DIR)/lib/$(SHAREDLIBmdsutils)

