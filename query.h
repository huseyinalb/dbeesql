#ifndef __dbeesql__updatequery__
#define __dbeesql__updatequery__
#include "table.h"
#include <list>

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
