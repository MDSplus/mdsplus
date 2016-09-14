#include <mdsobjects.h>
#include "testing.h"

using namespace MDSplus;



int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    BEGIN_TESTING(ExpressionCompile);
           
    AutoPointer<Data> data = MDSplus::compile("[1,2,3]");
    // TODO: adds more tests .. //
    
        
    
    END_TESTING;            
}

