#include "query.h"


Query::Query(string table_name, list<Condition*> *conditions, list<SetAction*> *setActions): table_name(table_name), conditions(conditions), setActions(setActions) {
    insert_values = (list<string*>*) 0;
}
Query::Query(string table_name, list<string*> *insert_values): table_name(table_name), insert_values(insert_values) {
    setActions = (list<SetAction*>*) 0;
    conditions = (list<Condition*>*) 0;
}
template< typename T >
struct delete_ptr : public std::unary_function<bool,T>
{
    bool operator()(T*pT) const { delete pT; return true; }
};

Query::~Query() {
    if (conditions)
        std::for_each( conditions->begin(), conditions->end(), delete_ptr<Condition>() );
    if (setActions)
        std::for_each( setActions->begin(), setActions->end(), delete_ptr<SetAction>() );
    if (insert_values)
        std::for_each( insert_values->begin(), insert_values->end(), delete_ptr<string>() );
}