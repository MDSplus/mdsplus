include $(MAKE_OPTIONS)

OBJECTS = \
      ServerDispatchAction.o\
      ServerFreeDispatchTable.o\
      ServerSetDetailProc.o\
      ServerAbortServer.o\
      ServerDispatchClose.o\
      ServerGetInfo.o\
      ServerSetLogging.o\
      ServerBuildDispatchTable.o\
      ServerDispatchCommand.o\
      ServerMonitorCheckin.o\
      ServerStartServer.o\
      ServerCloseTrees.o\
      ServerDispatchPhase.o\
      ServerStopServer.o\
      ServerCreatePulse.o\
      ServerFindServers.o\
      ServerSendMessage.o

all: ../shlib ../shlib/libMdsServerShr.$(SHARE_TYPE) ../shlib/libMdsServerShr.a

../shlib/libMdsServerShr.$(SHARE_TYPE) : $(OBJECTS) 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) $(DLD) -L../shlib -lTdiShr -lTreeShr -lMdsShr -lipcs

../shlib/libMdsServerShr.a : $(OBJECTS)
	$(AR) -r $@ $(OBJECTS)

../shlib :
	mkdir ../shlib

clean :
	rm *.o
	rm ../shlib/libMdsServerShr.*
	rm .depend

.depend :
	- find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)
