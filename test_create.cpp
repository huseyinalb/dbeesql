#include "test_create.h"
#include "parser.h"

int main(){
    list<string> tokens = tokenize("create table table_test(name text, num int);");
    Table table = parse_create(tokens);
    assert(!table.table_name.compare("table_test"));
    ColumnListType::iterator iter = table.column_list.begin();
    assert(!(*iter).first.compare("name"));
    assert((*iter).second == 2);
    iter++;
    assert(!(*iter).first.compare("num"));
    assert((*iter).second == 1);
    table.create();
    
    tokens = tokenize("describe table table_test;");
    table = parse_describe(tokens);
    iter = table.column_list.begin();
    assert(table.column_list.size() == 2);
    assert(!(*iter).first.compare("name"));
    assert((*iter).second == 2);
    iter++;
    assert(!(*iter).first.compare("num"));
    assert((*iter).second == 1);
}