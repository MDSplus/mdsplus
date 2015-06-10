#include <mdsobjects.h>

#include "testing.h"
#include "testutils/unique_ptr.h"
#include "mdsplus/AutoPointer.hpp"

using namespace MDSplus;
using namespace testing;

void PrintOpcodes () {    
    for (int i=0; i<std::numeric_limits<short>::max(); ++i )
        std::cout << "opcode: " << i << "  =  "
                  << (Function*)unique_ptr<Function>(new Function(i,0,NULL)) 
                  << "\n";    
}



int main() {
    BEGIN_TESTING(Function);
            
    { // CTR
        Data * raw[200];
        for(int i=0; i<200; ++i) raw[i] = new Float32(i);    
        char dummy_op_code = 0;
        unique_ptr<Function> ctr = new Function(0,sizeof(dummy_op_code),&dummy_op_code,200,(char**)raw);
    }
        
    Data *args[2] = { new Int32(2), new Int32(2) };
    unique_ptr<Function> fun = new Function(2,2,(Data**)args);
    
    // std::cout << (Function*)fun << "\n";
    PrintOpcodes();
        
    
    { // clone //
        unique_ptr<Function> clone = (Function*)fun->clone();
    }
    
    { // TDI compile //
        std::stringstream ss; ss << (Function*)fun;
        //        TEST1(ss.str() == std::string("Build_Function(0, 0, 10, .5552368D0)"));
    }
    
    { // access //

    }
    
            
    END_TESTING;
}

