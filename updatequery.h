#ifndef __dbeesql__updatequery__
#define __dbeesql__updatequery__
#include "table.h"
#include <list>

class UpdateQuery
{
public:
    Table table;
    list<Condition> conditions;
    list<SetAction> setActions;
    UpdateQuery(Table table, list<Condition> conditions, list<SetAction> setActions);
};

#endif /* defined(__dbeesql__updatequery__) */
