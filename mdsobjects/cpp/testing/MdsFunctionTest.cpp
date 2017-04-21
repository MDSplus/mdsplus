#include <iostream>
#include <iomanip>

#include <mdsobjects.h>

#include "testing.h"
#include "testutils/testutils.h"
#include "testutils/unique_ptr.h"
#include "mdsplus/AutoPointer.hpp"

using namespace MDSplus;
using namespace testing;

void PrintOpcodes () {    
    for (int i=0; i<456 /*std::numeric_limits<short>::max()*/; ++i )
        std::cout << "opcode: " << i << "  =  "
                  << (Function*)unique_ptr<Function>(new Function(i,0,NULL)) 
                  << "\n";    
}



int main(int argc UNUSED_ARGUMENT, char **argv) {
    BEGIN_TESTING(Function);
            
    if( argc > 1 && std::string(argv[1]) == std::string("print")) {
        PrintOpcodes(); exit(0);
    }
    
    
    { // CTR
        Data * raw[200];
        for(int i=0; i<200; ++i) raw[i] = new Float32(i);    
        char dummy_op_code = 0;
        unique_ptr<Function> ctr = new Function(0,sizeof(dummy_op_code),&dummy_op_code,200,(char**)raw);
    }
        
    Data *args[2] = { new Int32(555), new Float32(0.2368) };
    unique_ptr<Function> fun = new Function(38/*ADD*/, 2, (Data**)args);    
    unique_ptr<Data> ans = fun->evaluate();
    // std::cout << (Function*)fun << " = " << std::setprecision(7) << ans->getFloat() << "\n";

    TEST1( areSame(ans->getFloat(),(float)555.2368) );
    
    { // clone //
        unique_ptr<Function> clone = (Function*)fun->clone();
        unique_ptr<Data> ans = clone->evaluate();
        TEST1( areSame(ans->getFloat(),(float)555.2368) );        
    }
    
    { // TDI compile //
        std::stringstream ss; ss << (Function*)fun;
        TEST1(ss.str() == std::string("555 + .2368"));
    }
    
    { // access //
        fun->setArgAt(new Int32(1),0);
        fun->setArgAt(new Int32(2),1);        
        TEST1(unique_ptr<Data>(fun->getArgumentAt(0))->getInt() == 1);
        TEST1(unique_ptr<Data>(fun->getArgumentAt(1))->getInt() == 2);        
        TEST1(unique_ptr<Data>(fun->evaluate())->getInt() == 3 );        
    }
    
            
    END_TESTING;
}

