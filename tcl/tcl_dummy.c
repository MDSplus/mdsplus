#include        <stdio.h>

#define dummy(a) \
    int a(){printf("Routine %s not implemented yet\n",#a);return 0;}

dummy(TclLink)
dummy(TreeMarkIncludes)
dummy(TclPutExpression)
dummy(TclCommand)
dummy(TclRundownTree)
dummy(TclSetEvent)
dummy(TclShowDB)
dummy(TclWfevent)


	/* Some MDS and TREESHR routines not implemented on unix ...	*/
#ifndef __vms
dummy(ServerAbortServer)
dummy(ServerBuildDispatchTable)
dummy(ServerCloseTrees)
dummy(ServerCreatePulse)
dummy(ServerDispatchAction)
dummy(ServerDispatchClose)
dummy(ServerDispatchCommand)
dummy(ServerDispatchPhase)
dummy(ServerFailedEssential)
dummy(ServerFindServers)
dummy(ServerFreeDispatchTable)
dummy(ServerGetInfo)
dummy(ServerSetLogging)
dummy(ServerStartServer)
dummy(ServerStopServer)
/*dummy(TreeSetNoSubtree)		/*  */
#endif
