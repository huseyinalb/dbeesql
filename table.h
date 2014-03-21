
#ifndef __dbeesql__table__
#define __dbeesql__table__

#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <cstdio>
#include <sstream>
#include "condition.h"
#include <cstdlib>
#include "set_actions.h"
#include "query.h"
#include <set>

using namespace std;

// column_name, column_type
typedef list< pair<std::string, int> > ColumnListType;
typedef pair<std::string, int> ColumnType;
enum type_ids {INT_TYPE= 1, TEXT_TYPE = 2};

class Table
{
public:
    string table_name;
    ColumnListType column_list;
    list< list< void * > > values;
    Table(string filename);
    Table(string table_name, ColumnListType column_list);
    void fetch_content();
    void create();
    void insert(list<string*>* values);
    string describe();
    list< list< void* > > filter(list<Condition*> *conditions);
    string process_according_rows(Query *query, list< list< void* > >::iterator (*process_row)(Table*, Query*, list< list< void* > >::iterator));
    string print_rows(list< list< void* > >, list<string*>* column_names);
    ~Table();
    void suspend_content();
    int drop();
};
list< list< void* > >::iterator remove_row(Table* table, Query* query, list< list< void* > >::iterator rows_iter);
list< list< void* > >::iterator update_row(Table* table, Query* query, list< list< void* > >::iterator rows_iter);
#endif /* defined(__dbeesql__table__) */
