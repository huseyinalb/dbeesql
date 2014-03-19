#ifndef __dbeesql__condition__
#define __dbeesql__condition__

#include <iostream>

using namespace std;

enum cond_ids {EQUALS= 1, BIGGER = 2, SMALLER=3};

class Condition
{
public:
    string column_name;
    int condition;
    string value;
};

#endif /* defined(__dbeesql__condition__) */
