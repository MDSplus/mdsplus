
#include <iostream>

#include "testing.h"


class MyTestPersonalClass {
    int a,b;
public:
    MyTestPersonalClass() : a(0),b(123) {}
    const int & A() const { return a; }
    int & A() { return a; }
};



int main(int argc, char *argv[])
{
    BEGIN_TESTING(test cpp);
    std::cout << "Hello Test!\n";
        
    MyTestPersonalClass obj;
    TEST1(obj.A() == 0);
    
    END_TESTING;
    return 0;
}
