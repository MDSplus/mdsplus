#include <mdsobjects.h>
#include "testing.h"

using namespace MDSplus;



int main(int argc UNUSED_ARGUMENT, char *argv[] UNUSED_ARGUMENT)
{
    BEGIN_TESTING(ExpressionCompile);
           
    AutoPointer<Data> data = MDSplus::compile("[1,2,3]");
    // TODO: adds more tests .. //
    
        
    
    END_TESTING;            
}

