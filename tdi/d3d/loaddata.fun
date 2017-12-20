/*

   LOADDATA.FUN

	This TDI routine is used for dispatching between shot analysis codes at DIII-D.  It is to be called 
	from action nodes within the D3D tree and can be used in several different ways depending on the 
	provided arguments.

	Arguments:  
		    REQUIRED    _actionpath - path to the calling aciton node from the top of the d3d tree
		    OPTIONAL IN _machine    - machine to dispatch the command to
		    OPTIONAL IN _command    - command to dispatch to the machine

	Use Cases:  
		    1) LOADDATA("<node path>") 
   
		       Will assume that the analysis code is an IDL routine that is to be dispatched to the 
		       default machine specified in this tdi function.  It assumes that any further information
		       will be obtainable from the action node's subnodes (e.g. the analysis code's path).

		    2) LOADDATA("<node path>","<machine>") 

		       Same as use case #1 but will be dispatched to the machine specified instead of the default.

		    3) LOADDATA("<node path>","<machine>","<command>") - 

		       Will assume that the analysis code is command line executable (e.g. bash or csh script)
		       and that it can simply be dispatched directly to the machion.  In this case, the 
		       developer is responsible for any environment setup, logfile handling, posting to DAM, 
		       and sending of events back to atlas.gat.com.  These things are normally by the loaddata
		       wrapper when dispatching IDL codes but must be handled on a case by case basis when not 
 		       using IDL via use case #1 or #2.	

*/


PUBLIC FUN LOADDATA(IN _actionpath, OPTIONAL IN _machine, OPTIONAL IN _command)
{
   /* Send the code to metis.gat.com if the _machine was not specified. */ 
   IF (NOT PRESENT(_machine)) _machine = "metis";

   /* Use loaddata if the _command was not specified.  Be careful though, */
   /* since we have a slightly different environment on metis vs talos.   */
   IF (NOT PRESENT(_command)) 
   {     
      IF (UPCASE(EXTRACT(0,5,_machine)) == "METIS") 
      {
         _command = "/f/mdsplus/dispatching/loaddata "//$SHOT//" "//$EXPT//" "//_actionpath//" >&/dev/null &";
      } 
      IF (UPCASE(EXTRACT(0,5,_machine)) == "TALOS")
      { 
         _command = "loaddata "//$SHOT//" "//$EXPT//" "//_actionpath//" >&/dev/null &";
      }
      IF (_command == "")
      {
         _command = "/f/mdsplus/dispatching/loaddata "//$SHOT//" "//$EXPT//" "//_actionpath//" >&/dev/null &";
      }
   }

   /* Send the analysis code to the specified machine */
   _cmd = "/usr/bin/ssh -q "//_machine//" "//_command;
   write (*,"ACTION: "//_actionpath); 
   write (*,"CMD: "//_cmd);
   SPAWN(_cmd);
   return(1);

}	
