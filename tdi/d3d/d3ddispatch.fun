/*
   D3DDISPATCH.FUN

   This TDI function checks the (D3DDISPATCH) current shot and sends the phase to
   the relevant dispatcher port, as below:

      - port 8002: current d3ddispatch shot
      - port 8012: current d3ddispatch shot - 1
      - else: error, shot is too old to send to either dispatcher

   The return value of 1 indicates success.
                       0 indicates failure.
*/

PUBLIC FUN D3DDISPATCH(IN _PHASE, IN _SHOT)
{
   _D3DDISPATCH_SHOT = CURRENT_SHOT("D3DDISPATCH");
  
   IF ( _SHOT == _D3DDISPATCH_SHOT ) {
      WRITE(*,"D3DDISPATCH: Executing "//_PHASE//" for shot "//_SHOT//" at "//DATE_TIME());
      TCL("DISPATCH/COMMAND/SERVER=atlas.gat.com:8002 DISPATCH/PHASE "//_PHASE);
   } ELSE {
      IF ( _SHOT == _D3DDISPATCH_SHOT - 1 ) {
         TCL("DISPATCH/COMMAND/SERVER=atlas.gat.com:8012 @/usr/local/mdsplus/dispatching/missedphase.tcl "//_PHASE//" "//ADJUSTL(_SHOT));
      } ELSE { 
         WRITE(*,"D3DDISPATCH: Ignored "//_PHASE//" for shot "//_SHOT//" (current shot is "//_D3DDISPATCH_SHOT//").");
         RETURN(0);
      }
   }
   RETURN(1);
}
