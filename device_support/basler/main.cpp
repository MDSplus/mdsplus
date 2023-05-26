#include <mdsobjects.h>
using namespace MDSplus;
#include <sys/time.h>
#include <unistd.h>

#include "BASLERACA.h"
#include <cammdsutils.h>
#include <camstreamutils.h>


#define USETHECAMERA  //this let to use the camera or if not defined to read data from a pulse file.


int main(int argc, char **argv )
{

  if((argv[1]==NULL) || (argv[2]==NULL) || (argv[3]==NULL))   // argv[4] is OPTIONAL
  { 
	printf("Please insert: 1)tree name 2)shot number 3)number of frame to acquire 4) Streaming Port (OPTIONAL)\n"); 
	exit(0); 
  }

//MDSPLUS
  int res;
  void *treePtr;
  res=camOpenTree(argv[1], atoi( argv[2] ), &treePtr);
  if(res==-1)
  {
    printf("Error opening tree...\n"); 
	exit(0); 
  }

  Tree *tree; 
  TreeNode *node;
  TreeNode *nodeMeta;
  int framesNid, timebaseNid, framesMetadNid, frame0TimeNid;

  try
  {
    tree = (Tree *)treePtr;
    node=tree->getNode((char *)"\\BASLER::TOP:BASLER:FRAMES"); 		
    framesNid=node->getNid();	
    node=tree->getNode((char *)"\\BASLER::TOP:BASLER.TIMING:TIME_BASE"); 		
    timebaseNid=node->getNid();						
    node=tree->getNode((char *)"\\BASLER::TOP:BASLER:FRAMES_METAD");  	
    framesMetadNid=node->getNid();	
    node=tree->getNode((char *)"\\BASLER::TOP:BASLER:FRAME0_TIME"); 		
    frame0TimeNid=node->getNid();	
  }catch ( MdsException *exc )
    { std::cout << "ERROR reading data" << exc->what() << "\n"; }

 // printf("frame node path: %s\n", node->getPath());
//MDSPLUS END

//BASLER

    int camHandle;
    baslerOpen("192.168.54.96", &camHandle);  //put camera ip

    int width=0;
    int height=0;
    int payloadSize=0;

    setTreeInfo( camHandle, treePtr, framesNid, timebaseNid, framesMetadNid, frame0TimeNid);
    startAcquisition(camHandle, &width, &height, &payloadSize); 
    startFramesAcquisition(camHandle);
    stopFramesAcquisition(camHandle);
    stopAcquisition(camHandle); 
    baslerClose(camHandle);

}
