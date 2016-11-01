#include <stdlib.h>
#include <mdsobjects.h>

#ifdef _WIN32
#include <windows.h>
#endif

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

int main(int argc UNUSED_ARGUMENT, char *argv[] UNUSED_ARGUMENT)
{
    BEGIN_TESTING(MDSTreeSuppression);
#ifdef _WIN32
    _putenv_s("leaktest_path",".");
#else
    setenv("leaktest_path",".",1);
#endif
    
    Tree * tree = new Tree("leaktest",-1,"NEW");    
    tree->write();
    delete tree;

    tree = new Tree("leaktest",-1,"EDIT");

    TreeNode * node = tree->addNode("dummy_node","NUMERIC");
    delete node;
      
    TreeNode * node2 = tree->addNode("dummy_node2","TEXT");
    delete node2;
        
    tree->write();
    delete tree;
    END_TESTING;
}

