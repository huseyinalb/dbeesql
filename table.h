
#ifndef __dbeesql__table__
#define __dbeesql__table__

#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <cstdio>

using namespace std;

typedef list< pair<std::string, int> > ColumnListType;

class Table
{
public:
    string table_name;
    ColumnListType column_list;
    int column_num;
    Table(string filename);
    Table(string table_name, ColumnListType column_list);
    void suspend();
    int drop();
};
#endif /* defined(__dbeesql__table__) */
