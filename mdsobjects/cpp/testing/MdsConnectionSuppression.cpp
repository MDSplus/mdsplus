
#include <unistd.h>
#include <fstream>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

#include <mdsobjects.h>


#include "testing.h"
#include "testutils/testutils.h"
#include "testutils/unique_ptr.h"
#include "testutils/MdsIpInstancer.h"



using namespace MDSplus;
using namespace testing;



int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    
    Connection cnx((char*)"udt://localhost:8000",0);    
    cnx.openTree((char*)"test_tree",-1);
    cnx.closeAllTrees();
    
}
