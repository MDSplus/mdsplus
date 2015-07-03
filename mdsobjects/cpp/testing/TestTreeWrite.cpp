#include <stdlib.h>
#include <mdsobjects.h>



#include <dbidef.h>
#include <ncidef.h>
#include <mdstypes.h>
#include <usagedef.h>
#include <treeshr.h>



#include "testing.h"
#include "testutils/unique_ptr.h"
#include "mdsplus/AutoPointer.hpp"

using namespace MDSplus;
using namespace testing;



// this seems to catch a leak in treeshr

int main(int argc, char *argv[])
{
    setenv("leaktest_path",".",1);
    
    Tree * tree = new Tree("leaktest",-1,"NEW");    
    Tree * tree2 = new Tree("leaktest",-1,"EDIT");

    TreeNode * node = tree->addNode("dummy_node","NUMERIC");
    delete node;
      
    TreeNode * node2 = tree2->addNode("dummy_node","TEXT");
    delete node2;
        
    tree->write();
    tree2->write();       
    
    delete tree2;
    delete tree;       
    return 0;
}

 
//static int dbiSet(void * ctx, short int code, bool value) {
//	int len;
//	int intVal = value ? 1 : 0;
//	struct dbi_itm dbiList[] = {
//			{ sizeof(int), code, &intVal, &len },
//			{ 0, DbiEND_OF_LIST, 0, 0 }
//	};

//	int status = _TreeSetDbi(ctx, dbiList);
//    return status;
//}

//static void check_status(int status) {
//    if(!(status & 1)) {
//        printf("Error: %s \n", MdsGetMsg(status) );
//        throw MdsException(status);
//    }
//}

//int main(int argc, char *argv[])
//{
//    setenv("leaktest_path",".",1);        
    
//    int nid;
//    void *ctx = NULL;
//    // opening a new Tree or for editing //
//    check_status( _TreeOpenNew(&ctx, "leaktest", -1) );
    
//    // do something in that //
////    check_status( _TreeAddNode(ctx, "node", &nid, TreeUSAGE_ANY) );
    
//    // or something like ...
//    check_status( dbiSet(ctx, DbiVERSIONS_IN_MODEL, true) );
    
//    // finally close tree //
////    check_status( _TreeWriteTree(&ctx,"leaktest",-1) );
//    _TreeQuitTree(&ctx,"leaktest",-1);
////    check_status( _TreeClose(&ctx, "leaktest", -1) );    
    
//    TreeFreeDbid(ctx);
    
//    ctx = NULL;
//    check_status( _TreeOpenEdit(&ctx, "leaktest", -1) );
//    check_status( _TreeClose(&ctx, "leaktest", -1) );    

//    TreeFreeDbid(ctx);
    
//}
