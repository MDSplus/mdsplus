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
// if windows skip this test .. //
#ifdef _WIN32
#include "testing.h"
int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused))){
 SKIP_TEST("Connection test requires fork")
}
#else


#include <unistd.h>
#include <fstream>
#include <sys/types.h>
#include <signal.h>


#include <sys/wait.h>
#include <mdsobjects.h>


#include "testutils/testutils.h"
#include "testutils/unique_ptr.h"
#include "testutils/MdsIpInstancer.h"
#include "testing.h"


//
// TODO: Finish tests with PutMany and GetMany ...
//

using namespace MDSplus;
using namespace testing;




void test_tree_open(const char *prot)
{
    MdsIpInstancer mdsip(prot);
    usleep(500000);

    // get address form instancer for the specified protocol //
    std::string addr = mdsip.getAddress();

    std::cout << "attempt to connect to: " << addr << "\n";
    Connection cnx(const_cast<char*>(addr.c_str()),0);

    // test client-server communication //
    unique_ptr<Data> data = cnx.get("ZERO(10)");
    TEST1( AutoString(data->getString()).string
           == "[0.,0.,0.,0.,0.,0.,0.,0.,0.,0.]" );

    // test tree opening //
    cnx.openTree((char*)"test_tree",1);

    Data *args[] = { new Int32(5552368),
                     new Float64(111.234) };

    cnx.put("test_cnx",(char*)"$+10",args,1);
    cnx.put("test_cnx",(char*)"[$1+10, $2]",args,2);
    cnx.put("test_cnx",(char*)"5552368",NULL,0);

    deleteData(args[0]);
    deleteData(args[1]);

    data = cnx.get("test_cnx");
    TEST1( data->getInt() == 5552368 );

    // colsing tree //
    cnx.closeTree((char*)"test_tree",1);
}






////////////////////////////////////////////////////////////////////////////////
//  main  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



int main(int argc UNUSED_ARGUMENT, char *argv[] UNUSED_ARGUMENT)
{
    TEST_TIMEOUT(15);
    BEGIN_TESTING(Connection);

    setenv("test_tree_path",".",1);

    ////////////////////////////////////////////////////////////////////////////////
    //  Generate Tree  /////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    // this makes the test_tree in a separate process so that all static
    // variables instanced are destroied when the child ends.
    pid_t pid = fork();
    if(pid == 0)
    {
        {
            unique_ptr<Tree> tree = new Tree("test_tree",-1,"NEW");
            unique_ptr<TreeNode>(tree->addNode("test_cnx","NUMERIC"));
            tree->write();
            tree->edit(false);
            tree->createPulse(1);
        }
        exit(0);
    }
    else {
        // wait for child to finish the tree generation //
        int status;
        while (-1 == waitpid(pid, &status, 0));
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            std::cerr << "Process " << " (pid " << pid << ") failed" << "\n";
            exit(1);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    //  TEST CONNECTION TO REMOTE TREE  ////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    // tcp //
    test_tree_open("tcp");

    // udt //
    
#ifndef __ARM_ARCH
    test_tree_open("udt");
#endif

    END_TESTING;
}

#endif
