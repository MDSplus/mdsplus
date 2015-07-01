#include <stdlib.h>
#include <mdsobjects.h>

#include "testing.h"
#include "testutils/unique_ptr.h"
#include "mdsplus/AutoPointer.hpp"

using namespace MDSplus;
using namespace testing;



// this seems to catch a leak in treeshr

int main(int argc, char *argv[])
{
    setenv("test_tree_path",".",1);
    Tree * tree = new Tree("test_tree",-1,"NEW");    
    TreeNode * node = tree->addNode("dummy_node","ANY");
    
    delete node;
    delete tree;       
    return 0;
}


