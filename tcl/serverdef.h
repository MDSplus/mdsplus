/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
int ServerAbortServer(char *srvnam	/* <r> server name                      */
		      , int flushFlag	/* <r> flag                             */
    );
int ServerBuildDispatchTable(char *wildcard	/* <r:opt> wildcard characters?         */
			     , char *monitor	/* <r:opt> Monitor switch (text)?       */
			     , void **dispatch_table	/* <w> dispatch table addr              */
    );
int ServerCloseTrees(char *srvnam	/* <r> Server name                      */
    );
int ServerCreatePulse(int efn, char *srvnam, char *treenam, int ishot, void (*astRtn) ()
		      , void *astprm, int *retStatus, int *netId, void (*linkdown_handler) ()
		      , void (*before_ast) ()
    );
int ServerDispatchAction(int efn	/* <r> event flag number                */
			 , char *srvnam	/* <r> server name                      */
			 , char *treenam	/* <r> tree name                        */
			 , int shot	/* <r> shot id number                   */
			 , int nid	/* <r> node id                          */
			 , void (*ast) ()	/* <r> ast routine                  */
			 , void *astprm	/* <r> ast parameter                    */
			 , int *sts	/* <w:opt> return status                */
			 , int *netid	/* <w> net id?                          */
			 , void (*link_down) ()	/* <r:opt> addr of routine          */
			 , void (*before_ast) ()	/* <r:opt> "before-ast" routine ?   */
    );
int ServerDispatchClose(void *dispatch_table	/* <r> table address                    */
    );
int ServerDispatchCommand(int efn	/* <r> event flag num                   */
			  , char *srvnam	/* <r> server name                      */
			  , char *cli	/* <r> cli name?                        */
			  , char *command	/* <r> command to execute               */
			  , void (*astrtn) ()	/* <r:opt> addr of ast routine              */
			  , void *astprm	/* <r:opt> ast parameter                */
			  , int *status	/* <w> status of completed command      */
			  , int *netid	/* <w> net id ???                       */
			  , void (*link_down) ()	/* <r:opt> addr of routine  */
			  , void (*before_ast) ()	/* <r:opt> "before-ast" routine ?   */
    );
int ServerDispatchPhase(int efn	/* <r> event flag num                   */
			, void *dispatch_table	/* <r> table address                    */
			, char *phase	/* <r> phase name                       */
			, int noactionFlag	/* <r> flag                             */
			, int synchFlag	/* <r> flag                             */
			, void (*output_rtn) ()	/* <r> addr of output routine               */
			, char *monitor	/* <r:opt> "monitor" string?            */
    );
int ServerFindServers(void **ctx	/* <m> context variable                 */
		      , char *srch	/* <r> search str, may include wildcards */
		      , struct descriptor *dsc_srvnam	/* <w> descriptor for server name    */
    );
int ServerFreeDispatchTable(void *dispatch_table	/* <r> table address                    */
    );
int ServerGetInfo(int efn	/* <r> event flag num                   */
		  , int fullFlag	/* <r> flag                             */
		  , char *srvnam	/* <r> server name                      */
		  , struct descriptor *response	/* <w> requested info, returned         */
    );
int ServerSetLogging(char *srvnam	/* <r> server name                      */
		     , char logging	/* <r> flag                             */
    );
int ServerStartServer(char *srvnam	/* <r> server name                      */
    );
int ServerStopServer(char *srvnam	/* <r> server name                      */
    );
