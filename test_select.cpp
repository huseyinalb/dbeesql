#include "test_select.h"
#include "parser.h"

int main(){
    list<string> tokens = tokenize("insert into table_test values(deneme, 1);");
    pair< list<string>, string> res = parse_insert(tokens);
    list<string> values = res.first;
    string table_name = res.second;
    assert(!table_name.compare("table_test"));
    list<string>::iterator iter = values.begin();
    assert(!(*iter).compare("deneme"));
    iter++;
    
    assert(!iter->compare("1"));

    run_insert(values, table_name);

    tokens = tokenize("select * from table_test;");
    try {
        Table table = parse_select(tokens);
        assert(!table.table_name.compare("table_test"));
        string response = run_select(table);
        cout << response << endl;
        assert(table.values.size() == 0);
    } catch (const char * message) {
        cout << message << endl;
    }
}