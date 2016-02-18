
#include <unistd.h>

#include <mdsobjects.h>


#include "testing.h"
#include "testutils/testutils.h"
#include "testutils/unique_ptr.h"

using namespace MDSplus;
using namespace testing;

class NullEvent : public Event {
public:
    NullEvent(const char *name) : Event((char*)name) {}
        
    void run()
    {
        char *name = getName();                                     //Get the name of the event
        AutoString date(unique_ptr<Uint64>(getTime())->getDate());  //Get the event reception date 
        std::cout << "RECEIVED EVENT " << name << " AT " << date.string << "\n";
    }    
};


class RawEvent:public Event
{
    std::string test_str;
public:
    RawEvent(const char *name, std::string str) : 
        Event((char *)name),
        test_str(str)
    {}
    
    void run()
    {
        size_t bufSize;
        char *name = getName();                                     //Get the name of the event
        AutoString date(unique_ptr<Uint64>(getTime())->getDate());  //Get the event reception date 
        const char *str = getRaw(&bufSize);                         //Get raw data
        std::cout << "RECEIVED EVENT " << name << " AT " << date.string << " WITH RAW  " << str << "\n";
        TEST1( std::string(str) == test_str );        
    }
};


class DataEvent:public Event
{
    unique_ptr<Data> test_data;
public:
    DataEvent(const char *name, Data *data) : 
        Event((char *)name),
        test_data(data)
    {}
    
    void run()
    {        
        char *name = getName();                                     //Get the name of the event
        AutoString date(unique_ptr<Uint64>(getTime())->getDate());  //Get the event reception date 
        unique_ptr<Data> data = getData();                          //Get data
        std::cout << "RECEIVED EVENT " << name << " AT " << date.string 
                  << " WITH DATA  " << AutoString(data->getString()).string 
                  << "\n";
        TEST1( AutoString(test_data->getString()).string == AutoString(data->getString()).string );
    }
};



int main(int argc, char *argv[])
{
    BEGIN_TESTING(Event);
#   ifdef _WIN32
    SKIP_TEST("Event test requires fork")
#   else 
    setenv("UDP_EVENTS","yes",1);
    char *evname = (char *)alloca(30);
    sprintf(evname,"test_event_%d",getpid());    
    {        
        if(fork()) {            
            NullEvent ev(evname);
            ev.wait();
        } 
        else {            
            sleep(1);
            Event::setEvent(evname);
            exit(0);
        }            
    }
    
    
    {
        static std::string str("test string to be compared");
        
        if(fork()) {
            RawEvent ev(evname,str.c_str());
            size_t buf_len = 0;
            const char *buf = ev.waitRaw(&buf_len);
            TEST1( std::string(str) == std::string(buf) );
        }
        else {            
            sleep(1);            
            Event::setEventRaw(evname,str.size(),(char*)str.c_str());
            exit(0);
        }
    }
    
    
    { // DATA EVENT //
        static unique_ptr<String> str = new String("test string to be compared");
        
        if(fork()) {
            DataEvent ev(evname,str->clone());
            unique_ptr<Data> data = ev.waitData();
            TEST1( AutoString(data->getString()).string == AutoString(str->getString()).string );            
        }
        else {                        
            sleep(1);
            Event::setEvent(evname,str);
            exit(0);
        }
    }    
    
#   endif
    END_TESTING;
}



