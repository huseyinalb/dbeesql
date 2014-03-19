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
        pair< Table, list<Condition> > res = parse_select(tokens);
        Table table = res.first;
        assert(!table.table_name.compare("table_test"));
        string response = run_select(table, res.second);
        cout << response << endl;
        assert(table.values.size() == 0);
                table.table_name = "2";
    } catch (const char * message) {
        cout << message << endl;
    }
    tokens = tokenize("update table_test set name = deneme1 where name = deneme;");
    try {
        UpdateQuery updateQuery = parse_update(tokens);
        Table table = updateQuery.table;
        assert(!table.table_name.compare("table_test"));
        table.fetch_content();
        string response = run_update(table, updateQuery.conditions, updateQuery.setActions);
        cout << response << endl;
        table.table_name = "3";
    } catch (const char * message) {
        cout << message << endl;
    }
/*    tokens = tokenize("select * from table_test where name = deneme1;");
    try {
        pair< Table, list<Condition> > res = parse_select(tokens);
        Table table = res.first;
        assert(!table.table_name.compare("table_test"));
        table.fetch_content();
        string response = run_select(table, res.second);
        cout << response << endl;
        assert(table.values.size() == 1);
    } catch (const char * message) {
        cout << message << endl;
    }
  */
    
}