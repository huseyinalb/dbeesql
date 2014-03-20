#include "test_select.h"
#include "parser.h"

int main(){
    try {
    list<string> tokens = tokenize("insert into table_test values(\"some value\", 1);");
    Query *query = parse_insert(tokens);
    list<string*> *values = query->insert_values;
    assert(!query->table_name.compare("table_test"));
    list<string*>::iterator iter = values->begin();
    assert(!(*iter)->compare("some value"));
    iter++;
    
    assert(!(*iter)->compare("1"));
    Table table(query->table_name);
    run_insert(table, query);
    delete query;
    } catch (const char* message) {
        cout << message << endl;
    }
    list<string> tokens = tokenize("select * from table_test;");
    try {
        Query *query = parse_select(tokens);
        Table table(query->table_name);
        assert(!table.table_name.compare("table_test"));
        string response = run_select(table, query);
        cout << response << endl;
        assert(table.values.size() == 1);
        delete query;
    } catch (const char * message) {
        cout << message << endl;
    }
    tokens = tokenize("update table_test set name = \"deneme1\" where name = \"some value\";");
    try {
        Query *query = parse_update(tokens);
        Table table(query->table_name);
        assert(!table.table_name.compare("table_test"));
        table.fetch_content();
        string response = run_update(table, query);
        delete query;
        cout << response << endl;
    } catch (const char * message) {
        cout << message << endl;
    }
    tokens = tokenize("select * from table_test where name = \"deneme1\";");
    try {
        Query *query = parse_select(tokens);
        Table table(query->table_name);
        assert(!table.table_name.compare("table_test"));
        string response = run_select(table, query);
        delete query;
        cout << response << endl;
        assert(table.values.size() == 1);
    } catch (const char * message) {
        cout << message << endl;
    }
    
}