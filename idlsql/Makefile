include $(MAKE_OPTIONS)

OBJECTS = \
        dynamic_syb.o \
        idlsql_syb.o \
        user_syb.o

EXPORTS = idlsql.export

SYBASE = /usr/local/sybase
IDL_DIR = /usr/local/rsi/idl

CFLAGS = -I../include -g -ieee  -I$(SYBASE)/include -I$(IDL_DIR)/external

idlsql : ../shlib ../shlib/libIdlSql.$(SHARE_TYPE)

../shlib/libIdlSql.$(SHARE_TYPE) : $(OBJECTS)
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -L$(IDL_DIR)/bin/bin.alpha -lidl -L$(SYBASE)/lib -no_so  -lsybdb  -so_archive -lc -lresolv -ldnet_stub

# sun 	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -L$(IDL_DIR)/bin/bin.solaris2.sparc -lidl -L$(SYBASE)/lib -B static -lsybdb  -B dynamic -lc -lresolv

../shlib : 
	mkdir ../shlib

clean :
	- rm *.o
	- rm ../shlib/libIdlSql.$(SHARE_TYPE)
	- rm .depend

.depend :
	- find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)
