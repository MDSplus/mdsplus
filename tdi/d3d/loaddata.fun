/*

   LOADDATA.FUN

	This TDI routine is used for dispatching between shot analysis codes at DIII-D.  It is to be called 
	from action nodes within the D3D tree.

	Arguments:  
		    _name       - the "name" of the code
		    _machine    - machine to dispatch the command to
		    _command    - command to dispatch to the machine

	Use Cases:  
		    1) LOADDATA("<name>","<machine>","<command> &") - 

		       Will assume that the analysis code is command line executable (e.g. bash or csh script)
		       and that it can simply be dispatched directly to the machion.  In this case, the 
		       developer is responsible for any environment setup, logfile handling, posting to DAM, 
		       and sending of events back to atlas.gat.com.  

*/


PUBLIC FUN LOADDATA(IN _name, IN _machine, IN _command)
{

   /* Send the analysis code to the specified machine */
   _cmd = "/usr/bin/ssh -q "//_machine//" "//_command;
   write (*,"ACTION: "//_name); 
   write (*,"CMD: "//_cmd);
   SPAWN(_cmd);
   return(1);

}	
