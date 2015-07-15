#include <unistd.h>
#include <fstream>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

#include <mdsobjects.h>


#include "testing.h"
#include "testutils/testutils.h"
#include "testutils/unique_ptr.h"
#include "testutils/mdsipmain.h"



using namespace MDSplus;
using namespace testing;


void test_tree_open(const char *prot) 
{
    MdsIpInstancer mdsip(prot);
    usleep(500);
    
    std::string addr = std::string(mdsip.getAddress()) + "";
    std::cout << "attempt to connect to: " << addr << "\n";    
    Connection cnx((char *)addr.c_str(),0);
    
    unique_ptr<Data> data = cnx.get("ZERO(10)");
    TEST1( AutoString(data->getString()).string == "[0.,0.,0.,0.,0.,0.,0.,0.,0.,0.]");
    
    
    cnx.openTree((char*)"test_tree",1);    
    
    Data *args[] = { new Int32(5552368),
                     new Float64(111.234) };
    
    cnx.put("test_cnx",(char*)"$+10",args,1);
    cnx.put("test_cnx",(char*)"[$1+10, $2]",args,2);
    cnx.put("test_cnx",(char*)"5552368",NULL,0);
    
    data = cnx.get("test_cnx");
    TEST1( data->getInt() == 5552368 );
    
    cnx.closeTree((char*)"test_tree",1);        
}


int main(int argc, char *argv[])
{
    BEGIN_TESTING(Connection);
        
    setenv("test_tree_path",".",1);

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
        int status;
        while (-1 == waitpid(pid, &status, 0));
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            std::cerr << "Process " << " (pid " << pid << ") failed" << "\n";
            exit(1);
        }    
    }
    
    // TEST CONNECTION TO REMOTE TREE //    
    test_tree_open("tcp");        
    test_tree_open("udt");

    

    
    
    END_TESTING;
}
