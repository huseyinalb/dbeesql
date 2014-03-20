#include "test_table.h"
#include "table.h"
#include "condition.h"
#include <list>
#include <assert.h>

int main(){
    ColumnListType column_list;
    ColumnType columnType1;
    columnType1.first = "name";
    columnType1.second = TEXT_TYPE;
    ColumnType columnType2;
    columnType2.first = "num";
    columnType2.second = INT_TYPE;
    column_list.push_back(columnType1);
    column_list.push_back(columnType2);
    Table table("test_table", column_list);
    table.create();

    
    list < string* > *values = new list<string*>;

    string *val2 = new string("val2");
    values->push_back(val2);

    string *val1 = new string("1");
    values->push_back(val1);
    table.insert(values);
    table.suspend_content();
    
    
    Table table2("test_table");
    table2.fetch_content();
    cout << table2.values.front().size() << endl;
    list< list < void* > >::iterator val_it= table2.values.begin();
    string* first_val = (string*)val_it->front();
    assert(!first_val->compare("val2"));
    val_it->pop_front();
    int* sec_val = (int*)val_it->front();
    val_it->pop_front();
    assert((*sec_val) == 1);
    assert(val_it->size() == 0);
    
    table2.fetch_content();
    cout << table2.values.front().size() << endl;
    assert(table2.values.front().size() == 2);
    Condition *condition1= new Condition;
    
    condition1->value = "val2";
    condition1->condition = EQUALS;
    condition1->column_name = "name";
    list<Condition*> *conditions = new list<Condition*>;
    conditions->push_back(condition1);
    list< list<void*> > rows = table2.filter(conditions);
    assert(rows.size() == 1);
    val_it= rows.begin();
    first_val = (string*)val_it->front();
    assert(!first_val->compare("val2"));
    sec_val = (int*)val_it->back();
    assert(*sec_val == 1);
}