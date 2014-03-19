
#ifndef __dbeesql__table__
#define __dbeesql__table__

#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <cstdio>
#include <sstream>

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
    void insert(list<string> values);
    string describe();
    string select();
    ~Table();
    void suspend_content();
    int drop();
};
#endif /* defined(__dbeesql__table__) */
