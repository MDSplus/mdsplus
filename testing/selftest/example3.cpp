
#include <iostream>

#include "testing.h"


class MyTestPersonalClass {
    int a,b;
public:
    MyTestPersonalClass() : a(0),b(123) {}
    const int & A() const { return a; }
    int & A() { return a; }
};



int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    BEGIN_TESTING(test cpp);
    std::cout << "Hello Test!\n";
        
    MyTestPersonalClass obj;
    TEST1(obj.A() == 0);
    
    END_TESTING;
}
