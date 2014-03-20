#ifndef __dbeesql__updatequery__
#define __dbeesql__updatequery__
#include <list>
#include <string>
#include "condition.h"
#include "set_actions.h"
using namespace std;

class Query
{
public:
    string table_name;
    list<Condition*> *conditions;
    list<SetAction*> *setActions;
    list<string *> *insert_values;
    Query(string table_name, list<Condition*> *conditions, list<SetAction*> *setActions);
    Query(string table_name, list<string*>* insert_values);
    ~Query();
};

#endif /* defined(__dbeesql__updatequery__) */
