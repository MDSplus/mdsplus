#include <mdsobjects.h>

#include "testing.h"
#include "testutils/unique_ptr.h"
#include "mdsplus/AutoPointer.hpp"

using namespace MDSplus;
using namespace testing;

////////////////////////////////////////////////////////////////////////////////
//  Range COMPOUND  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



int main() {
    BEGIN_TESTING(Range);
            
    { // CTR
        Data * raw[200];
        for(int i=0; i<200; ++i) raw[i] = new Float32(i);    
        char dummy_op_code = 0;
        unique_ptr<Range> ctr = new Range(0,sizeof(dummy_op_code),&dummy_op_code,200,(char**)raw);
    }
        
    unique_ptr<Range> range = new Range(new Int32(0), new Int32(5), new Float64(0.5));
    
    { // clone //
        unique_ptr<Range> clone = (Range*)range->clone();        
        clone->setBegin(new Int32(0));
        clone->setEnding(new Int32(1));
        clone->setDeltaVal(new Float32(1.));
        unique_ptr<Data>(clone->getBegin());
        unique_ptr<Data>(clone->getEnding());
        unique_ptr<Data>(clone->getDeltaVal());
    }
    
    { // TDI compile //
        std::stringstream ss; ss << (Range*)range;      
        TEST1(ss.str() == std::string("0 : 5 : .5D0"));
    }
    
    { // test TDI evaluated data //
        std::stringstream ss; ss << (Data*)unique_ptr<Data>(range->data());        
        TEST1(ss.str() == std::string("[0D0,.5D0,1D0,1.5D0,2D0,2.5D0,3D0,3.5D0,4D0,4.5D0,5D0]"));
    }
    
    { // access //
        range->setBegin(new Int32(0));
        range->setEnding(new Int32(-10));
        range->setDeltaVal(new Float32(-1.0));
        unique_ptr<Data>(range->getBegin());
        unique_ptr<Data>(range->getEnding());
        unique_ptr<Data>(range->getDeltaVal());
        std::stringstream ss; ss << (Data*)unique_ptr<Data>(range->data());
        TEST1(ss.str() == std::string("[0.,-1.,-2.,-3.,-4.,-5.,-6.,-7.,-8.,-9.,-10.]"));        
    }    
            
    END_TESTING;
}

