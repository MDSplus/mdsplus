/*
   Dependencies:
     stdio.h (for fprintf, sprintf)
     mdslib.h (for MdsValue, descr)
 */

#include <mdslib.h>
#include <stdio.h>
/* #define status_ok(status) (((status) & 1) == 1) */

  /*
    status_ok

    Description:
      Returns 1 if OK, 0 otherwise.  Status is OK if the
      LSB is set.

    Dependencies:
      none.
  */
  int status_ok( int status )
  {
    return ( (status & 1) == 1 );
  }

int main(int argc, char *argv[])
{
    int status;
    int shot = 3;
    
    int dtype_long = DTYPE_LONG;
    int dtype_cstring = DTYPE_CSTRING;
    int tstat, len, null=0;
    int bufsize = 1024;
    char buf[1024];
    int idesc = descr(&dtype_long, &tstat, &null);
    int sdesc = descr(&dtype_cstring, buf, &null, &bufsize);

     /* Open tree */
    status = MdsOpen("cryocon18i", &shot);
    if ( !status_ok(status) )
    {
      printf("Error shot number %d\n", shot); 
      fprintf(stderr,"Error shot number %d\n", shot);
      return -1;
    }
    printf("Status: Tree opened %d\n", status);

    puts("Demonstrating use of TCL() function.");
    /* status = MdsValue("TCL(\"set def cryo18i\",_output)", &idesc, &null, &len); */
    status = MdsValue("TCL(\"set def cryo18i\",_output)", &idesc, &null, &len);
    if ( !status_ok(status) )
    {
      printf("Error with set def command.\n");
      fprintf(stderr,"Error with set def command.\n");
      return -1;
    }
    printf("Status of TCL(\"set def\") = %i\n",tstat);
    
    /* If the command was successful, print its output. */
    if ( status_ok(tstat) )
    {
      status = MdsValue("_output", &sdesc, &null, &len);
      if ( !status_ok(status) )
      {
        fprintf(stderr,"Error getting _output from set def command.\n");
        return -1;
      }
      printf("Output of TCL(\"set def\") = %s\n", buf);
    }
    return 0;
 }
 