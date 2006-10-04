#include        <mds_stdarg.h>
#include        "tclsysdef.h"
#include <dbidef.h>

/***************************************************************
 * TclSetVersions:
 **************************************************************/

int TclSetVersions() {
  int   status=1;
  
  /*--------------------------------------------------------
   * Executable ...
   *-------------------------------------------------------*/
  switch (cli_present("MODEL")){
  case CLI_STS_PRESENT:
    status = TreeSetDbiItm(DbiVERSIONS_IN_MODEL,1);
    break;
  case CLI_STS_NEGATED:
    status = TreeSetDbiItm(DbiVERSIONS_IN_MODEL,0);
    break;
  }
  if (!(status & 1)) goto error;
  switch (cli_present("SHOT")){
  case CLI_STS_PRESENT:
    status = TreeSetDbiItm(DbiVERSIONS_IN_PULSE,1);
    break;
  case CLI_STS_NEGATED:
    status = TreeSetDbiItm(DbiVERSIONS_IN_PULSE,0);
    break;
  }
 error:
  if (!(status & 1)) {
    MdsMsg(status,0);
  }
  return status;
}


int TclShowVersions() {
  int in_model,in_pulse,status;
  DBI_ITM itmlst[] = {{4,DbiVERSIONS_IN_MODEL,&in_model,0},{4,DbiVERSIONS_IN_PULSE,&in_pulse,0},{0,0,0,0}};
  status = TreeGetDbi(itmlst);
  if (status & 1) {
    char msg[356];
    TclTextOut("");
    strcpy(msg,"   Versions are ");
    strcat(msg,in_model ? "enabled" : "disabled");
    strcat(msg," in the model file and ");
    strcat(msg,in_pulse ? "enabled" : "disabled");
    strcat(msg," in the shot file.");
    TclTextOut(msg);
    TclTextOut("");
  }
  else
    MdsMsg(status,0);
  return status;
}

    
