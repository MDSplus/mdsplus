#include <unistd.h>

#include <mdsobjects.h>

#include "testing.h"
#include "testutils/testutils.h"
#include "testutils/unique_ptr.h"

using namespace MDSplus;
using namespace testing;

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    
    {
        Event::setEvent((char*)"test_event");        
    }
    
    
    {
        unique_ptr<String> str = new String("test event string");
        Event::setEvent((char*)"test_event",str);
    }
    
    {
        std::string str;
        Event::setEventRaw((char*)"test_event",str.size(),(char*)str.c_str());
    }
    
    return 0;
}

