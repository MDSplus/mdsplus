include $(MAKE_OPTIONS)

OBJECTS = \
        dynamic_syb.o \
        idlsql_syb.o \
        user_syb.o

EXPORTS = idlsql.export

INCLUDEDIR = $(INCLUDEDIR) -I$(SYBASE)/include -I/usr/local/rsi/idl_5.2/external

idlsql : ../shlib ../shlib/libIdlSql.$(SHARE_TYPE)

../shlib/libIdlSql.$(SHARE_TYPE) : $(OBJECTS)
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -L$(IDL_BINPATH) -L$(SYBASE)/lib  -lidl -no_so -lsybdb -so_archive -lc -lresolv -ldnet_stub

../shlib : 
	mkdir ../shlib

clean :
	- rm *.o
	- rm ../shlib/libIdlSql.$(SHARE_TYPE)
	- rm .depend

.depend :
	- find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)
