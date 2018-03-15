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

#include "testutils/MdsIpInstancer.h"
#include <unistd.h>
#include <fstream>
#include <sys/types.h>
#include <signal.h>

#include <mdsobjects.h>

#include "testutils/testutils.h"
#include "testutils/unique_ptr.h"
#include "testing.h"

#ifdef _WIN32
#define setenv(name,val,extra) _putenv_s(name,val)
#endif

//
// TODO: Finish tests with PutMany and GetMany ...
//

using namespace MDSplus;
using namespace testing;




void test_tree_open(const char *prot, const unsigned short port)
{
    MdsIpInstancer mdsip(prot,port);

    // get address form instancer for the specified protocol //
    std::string addr = mdsip.getAddress();

    std::cout << "attempt to connect to: " << addr << "\n" << std::flush;
    unique_ptr<Connection> cnx = NULL;
    int retry = 5;
    for (;!cnx;) try {
      cnx = new Connection(const_cast<char*>(addr.c_str()));
    } catch (...) {
      if (--retry<0)
        throw;
      std::cout << "retry\n" << std::flush;
      usleep(500000);
    }
    // test client-server communication //
    unique_ptr<Data> data = cnx->get("ZERO(10)");
    TEST1( AutoString(data->getString()).string
           == "[0.,0.,0.,0.,0.,0.,0.,0.,0.,0.]" );

    data = cnx->get("DECOMPILE(`TreeShr->TreeDbid:P())");
    TEST0( AutoString(data->getString()).string
           == "Pointer(0)" );

    data = cnx->get("setTimeContext()");
    TEST1(data->getInt() == 1);

    // test tree opening //
    data = cnx->get("setenv('t_connect_path=.')");
    cnx->openTree((char*)"t_connect",1);

    Data *args[] = { new Int32(5552368),
                     new Float64(111.234) };

    cnx->put("test_cnx",(char*)"$+10",args,1);
    cnx->put("test_cnx",(char*)"[$1+10, $2]",args,2);
    cnx->put("test_cnx",(char*)"5552368",NULL,0);

    deleteData(args[0]);
    deleteData(args[1]);

    data = cnx->get("test_cnx");
    TEST1( data->getInt() == 5552368 );

    // colsing tree //
    cnx->closeTree((char*)"t_connect",1);
}


////////////////////////////////////////////////////////////////////////////////
//  main  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



int main(int argc UNUSED_ARGUMENT, char *argv[] UNUSED_ARGUMENT)
{
    TEST_TIMEOUT(30);
    setenv("t_connect_path",".",1);


    ////////////////////////////////////////////////////////////////////////////////
    //  Generate Tree  /////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    // this makes the t_connect in a separate process so that all static
    // variables instanced are destroied when the child ends.
    try {
      unique_ptr<Tree> tree = new Tree("t_connect",-1,"NEW");
      unique_ptr<TreeNode>(tree->addNode("test_cnx","NUMERIC"));
      tree->write();
      tree->edit(false);
      tree->createPulse(1);
    } catch (...) {
      std::cerr << "Error creating model tree";
    }

    ////////////////////////////////////////////////////////////////////////////////
    //  TEST CONNECTION TO REMOTE TREE  ////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    // local //
    BEGIN_TESTING(Connection local);
    test_tree_open("local",0);
    END_TESTING;

/*
// TODO: http test does not work
// http server requires setup

    // http //
    BEGIN_TESTING(Connection http);
    test_tree_open("http",0);
    END_TESTING;
*/

/*
// TODO: ssh test does not work (unknown user)
// ssh requires some setup for executing user
// $HOME/.ssh/{authorized_keys,id_rsa,id_rsa.pub,known_hosts}

    // ssh //
    BEGIN_TESTING(Connection ssh);
    test_tree_open("ssh",0);
    END_TESTING;
*/

    // tcp //
    BEGIN_TESTING(Connection tcp);
    test_tree_open("tcp",8601);
    END_TESTING;

    // udt //
    BEGIN_TESTING(Connection udt);
    test_tree_open("udt",8602);
    END_TESTING;

    // tcpv6 //
    BEGIN_TESTING(Connection tcpv6);
    test_tree_open("tcpv6",8603);
    END_TESTING;

    // udtv6 //
    BEGIN_TESTING(Connection udtv6);
    test_tree_open("udtv6",8604);
    END_TESTING;

/*
// TODO: gsi test does not work (gsi setup?)
// ERROR:Error connecting ---
//       globus_xio_gsi: gss_init_sec_context failed.
//
// ERROR:mdsip_accept_cp, open ---
//       globus_xio: The GSI XIO driver failed to establish a secure connection. The failure occured during a handshake read.

    // gsi //
    BEGIN_TESTING(Connection gsi);
    test_tree_open("gsi",8605);
    END_TESTING;
*/
}
