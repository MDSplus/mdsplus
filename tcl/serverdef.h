int   ServerAbortServer(
    char  *srvnam		/* <r> server name			*/
   ,int   flushFlag		/* <r> flag				*/
   );
int   ServerBuildDispatchTable(
    char  *wildcard		/* <r:opt> wildcard characters?		*/
   ,char  *monitor		/* <r:opt> Monitor switch (text)?	*/
   ,void  **dispatch_table	/* <w> dispatch table addr		*/
   );
int   ServerCloseTrees(
    char  *srvnam		/* <r> Server name			*/
   );
int   ServerCreatePulse(
    int   efn
   ,char  *srvnam
   ,char  *treenam
   ,int   ishot
   ,void  (*astRtn)()
   ,void  *astprm
   ,int   *retStatus
   ,int   *netId
   ,void  (*linkdown_handler)()
   ,void  (*before_ast)()
   );
int   ServerDispatchAction(
    int   efn			/* <r> event flag number		*/
   ,char  *srvnam		/* <r> server name			*/
   ,char  *treenam		/* <r> tree name			*/
   ,int   shot			/* <r> shot id number			*/
   ,int   nid			/* <r> node id				*/
   ,void  (*ast)()		/* <r> ast routine			*/
   ,void  *astprm		/* <r> ast parameter			*/
   ,int   *sts			/* <w:opt> return status		*/
   ,int   *netid		/* <w> net id?				*/
   ,void  (*link_down)()	/* <r:opt> addr of routine		*/
   ,void  (*before_ast)()	/* <r:opt> "before-ast" routine ?	*/
   );
int   ServerDispatchClose(
    void  *dispatch_table	/* <r> table address			*/
   );
int   ServerDispatchCommand(
    int   efn			/* <r> event flag num			*/
   ,char  *srvnam		/* <r> server name			*/
   ,char  *cli			/* <r> cli name?			*/
   ,char  *command		/* <r> command to execute		*/
   ,void  (*astrtn)()		/* <r:opt> addr of ast routine		*/
   ,void  *astprm		/* <r:opt> ast parameter		*/
   ,int   *status		/* <w> status of completed command	*/
   ,int   *netid		/* <w> net id ???			*/
   ,void  (*link_down)()		/* <r:opt> addr of routine	*/
   ,void  (*before_ast)()	/* <r:opt> "before-ast" routine ?	*/
   );
int   ServerDispatchPhase(
    int   efn			/* <r> event flag num			*/
   ,void  *dispatch_table	/* <r> table address			*/
   ,char  *phase		/* <r> phase name			*/
   ,int   noactionFlag		/* <r> flag				*/
   ,int   synchFlag		/* <r> flag				*/
   ,void  (*output_rtn)()	/* <r> addr of output routine		*/
   ,char  *monitor		/* <r:opt> "monitor" string?		*/
   );
int   ServerFindServers(
    void  **ctx			/* <m> context variable			*/
   ,char  *srch			/* <r> search str, may include wildcards */
   ,struct descriptor  *dsc_srvnam /* <w> descriptor for server name	*/
   );
int   ServerFreeDispatchTable(
    void  *dispatch_table	/* <r> table address			*/
   );
int   ServerGetInfo(
    int   efn			/* <r> event flag num			*/
   ,int   fullFlag		/* <r> flag				*/
   ,char  *srvnam		/* <r> server name			*/
   ,struct descriptor *response /* <w> requested info, returned		*/
   );
int   ServerSetLogging(
    char  *srvnam		/* <r> server name			*/
   ,char   logging		/* <r> flag				*/
   );
int   ServerStartServer(
    char  *srvnam		/* <r> server name			*/
   );
int   ServerStopServer(
    char  *srvnam		/* <r> server name			*/
   );
