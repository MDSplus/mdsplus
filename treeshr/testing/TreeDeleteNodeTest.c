/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <pthread.h>

// MDsplus //
#include <mdsshr.h>
#include <treeshr.h>
#include <usagedef.h>
#include <dbidef.h>

// testing //
#include "testing.h"


// inspired from the following C++ procedure:
//
// unique_ptr<Tree> tree = new Tree("test_tree",-1,"EDIT");
// tree->edit();
// unique_ptr<TreeNode>( tree->addNode("save_me_not","ANY"));        
// tree->write();
// tree->remove("save_me_not");
// tree->write();
//



int dbiTest(void *ctx, short int code) {
    int supports;    
    int len;
    struct dbi_itm dbiList[] = {
            { sizeof(int), code, &supports, &len },
            { 0, DbiEND_OF_LIST, 0, 0 }
    };
    
    int status = _TreeGetDbi(ctx, dbiList);
    TEST1(status & 1);
    
    return supports;
}

///
/// This test was meant to raise a helgrind warning due to abnormal thread
/// termination on exit from the main program. Helgrine complies with "Exiting
/// thread still holds 1 lock", 
/// see: stackoverflow.com/questions/3597985/check-for-threads-still-running-after-program-exits
///
/// 
int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    BEGIN_TESTING(Tree Delete Node);
    
    void *ctx = NULL;
    int shot = -1;
    int status;
    const char *tree_name = "tree_test";
    const char *node_name = "test_node";    
    setenv("tree_test_path",".",1);
    
    // open tree //
    status = _TreeOpenNew(&ctx, tree_name, shot);
    TEST1( status & 1 );
    TEST0( ctx == NULL );

    // add node //
    int nid;
	status = _TreeAddNode(ctx, node_name, &nid, TreeUSAGE_ANY);
    TEST1( status & 1 );
	    
    status = _TreeWriteTree(&ctx, tree_name, shot);
    TEST1( status & 1 );
    
    // remove node //
    int count;
	status = _TreeDeleteNodeInitialize(ctx, nid, &count, 1);
    TEST1( status & 1 );
    _TreeDeleteNodeExecute(ctx);
	
    status = _TreeWriteTree(&ctx, tree_name, shot);
    TEST1( status & 1 );
    
    
    // close tree //    
    int is_modified = dbiTest(ctx, DbiMODIFIED);    
    if( is_modified ) {
        status = _TreeQuitTree(&ctx, tree_name, shot);
        TEST1( status & 1 );
    } else {
        status = _TreeClose(&ctx, tree_name, shot);
        TEST1( status & 1 );
    }
    TreeFreeDbid(ctx);
    
    
    
    END_TESTING;
    return 0;
}
