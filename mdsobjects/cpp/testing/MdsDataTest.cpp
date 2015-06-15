
#include <string>
#include <cstring>

#include <mdsobjects.h>
#include <mdsplus/AutoPointer.hpp>

#include "testing.h"
#include "testutils/unique_ptr.h"

#include "MdsDataTest.h"



////////////////////////////////////////////////////////////////////////////////
//  WAITING TO BE TESTED:

//virtual int * getShape(int *numDim);
//virtual Data *getDimensionAt(int dimIdx);


using namespace MDSplus;
using namespace testing;

int main(int argc, char *argv[])
{
    BEGIN_TESTING(Mds Data Test);


    { // set in construction //
        Data * units = new String("units");
        Data * error = new String("error");
        Data * help = new String("help");
        Data * validation = new String("validation");

        unique_ptr<Data> data = new Int32(5552368,units,error,help,validation);

        { char * str = unique_ptr<Data>(data->getUnits())->getString(); TEST0( strcmp( str,"units") ); delete[] str; }
        { char * str = unique_ptr<Data>(data->getError())->getString(); TEST0( strcmp( str,"error") ); delete[] str; }
        { char * str = unique_ptr<Data>(data->getHelp())->getString(); TEST0( strcmp( str,"help") ); delete[] str; }
        { char * str = unique_ptr<Data>(data->getValidation())->getString(); TEST0( strcmp( str,"validation") ); delete[] str; }
    }

    { // set via accessor //
        Data * data = new Int32(5552368);

        data->setUnits(new String("units") );
        data->setError(new String("error") );
        data->setHelp(new String("help") );
        data->setValidation(new String("validation") );

        { char * str = unique_ptr<Data>(data->getUnits())->getString(); TEST0( strcmp( str,"units") ); delete[] str; }
        { char * str = unique_ptr<Data>(data->getError())->getString(); TEST0( strcmp( str,"error") ); delete[] str; }
        { char * str = unique_ptr<Data>(data->getHelp())->getString(); TEST0( strcmp( str,"help") ); delete[] str; }
        { char * str = unique_ptr<Data>(data->getValidation())->getString(); TEST0( strcmp( str,"validation") ); delete[] str; }

        deleteData(data);
    }


    { // nested setter getter calls //
        unique_ptr<Data> data1 = new Int32(5552368);
        unique_ptr<Data> data2 = new Int32(5552369);

        data1->setUnits(new String("units") );
        {
            Data * d1 = data1->getUnits();
            Data * d2 = data1->getUnits();
            data2->setUnits(d1);
            deleteData(d2);
        }

        unique_ptr<Data> d1 = data2->getUnits();
        { char * str = unique_ptr<Data>(d1)->getString(); TEST0( strcmp( str,"units") ); delete[] str; }
    }


    END_TESTING;
}

