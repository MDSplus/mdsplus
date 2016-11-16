#include <mdsobjects.h>

#include "testing.h"
#include "testutils/unique_ptr.h"
#include "mdsplus/AutoPointer.hpp"

using namespace MDSplus;
using namespace testing;

////////////////////////////////////////////////////////////////////////////////
//  WINDOW COMPOUND  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//class Window: public Compound
//{
//public:
//    Window(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) :
//    Window(Data *startidx, Data *endidx, Data *value_at_idx0, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)

//    Data *getStartIdx()
//    Data *getEndIdx()
//    Data *getValueAt0()
//    void setStartIdx(Data *startidx) {assignDescAt(startidx, 0);}
//    void setEndIdx(Data *endidx){assignDescAt(endidx, 1);}
//    void setValueAt0(Data *value_at_idx0) {assignDescAt(value_at_idx0, 2);}

//};


int main() {
    BEGIN_TESTING(Window);

    { // CTR
        Data * raw[200];
        for(int i=0; i<200; ++i) raw[i] = new Float32(i);
        char dummy_op_code = 0;
        unique_ptr<Window> ctr = new Window(0,sizeof(dummy_op_code),&dummy_op_code,200,(char**)raw);
    }

    unique_ptr<Window> win = new Window(new Int32(0), new Int32(10), new Float64(0.5552368));

    { // clone //
        unique_ptr<Window> clone = (Window*)win->clone();
        clone->setStartIdx(new Int32(0));
        clone->setEndIdx(new Int32(1));
        clone->setValueAt0(new Float32(1.));
        unique_ptr<Data>(clone->getStartIdx());
        unique_ptr<Data>(clone->getEndIdx());
        unique_ptr<Data>(clone->getValueAt0());
    }

    { // TDI compile //
        std::stringstream ss; ss << (Window*)win;
        TEST1(ss.str() == std::string("Build_Window(0, 0, 10, .5552368D0)"));
    }

    { // access //
        win->setStartIdx(new Int32(0));
        win->setEndIdx(new Int32(-10));
        win->setValueAt0(new Float32(-1.0));
        unique_ptr<Data>(win->getStartIdx());
        unique_ptr<Data>(win->getEndIdx());
        unique_ptr<Data>(win->getValueAt0());
    }


    END_TESTING;
}
