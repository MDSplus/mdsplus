/*
   Dependencies:
     stdio.h (for fprintf, sprintf)
     mdslib.h (for MdsValue, descr)
 */

#include "treeshr/treeshrp.h"
#include <mdslib.h>
#include <treeshr.h>
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
    /*char const tree="cryocon18i";
    char path = 'cryo18i';*/
    int read_only=1;
    
    void *dbid;
    int nidout=0;
    int startnid=0;
    // NID * nid_out = 0;
    // struct context *ctx = NULL;

    
    status = _TreeOpen(&dbid, "cryocon18i", shot, read_only);
    /*status = _TreeOpen(TreeCtx(), "cryocon18i", shot, read_only);*/
    if ( !status_ok(status) )
    {
      printf("Error shot number %d\n", shot); 
      fprintf(stderr,"Error shot number %d\n", shot);
      return -1;
    }
    // PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;

    // struct context {
    //     char *expt;
    //     int shot;
    //     int defnid;
    // };
    // if (IS_OPEN(dblist)) {
    //     ctx = malloc(sizeof(struct context));
    //     ctx->expt   = strcpy(malloc(strlen(dblist->experiment) + 1), dblist->experiment);
    //     ctx->shot   = dblist->shotid;
    // }
    
    _TreeGetDefaultNid(dbid, &nidout);
    startnid = nidout;
    int usage_mask=0xFFFF;
    void *ctx = NULL;
    // _TreeFindNode(dbid, "cryocon18i::top:cryo18i", &nidout);
    _TreeFindNodeWild(dbid, "cryocon18i::top:cryo18i", &nidout, &ctx, usage_mask);
    
    return 0;
 }
 