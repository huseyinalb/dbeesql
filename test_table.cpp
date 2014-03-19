#include "test_table.h"
#include "table.h"
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

    
    list < string > values;

    string val2 = "val2";
    values.push_back(val2);

    string val1 = "1";
    values.push_back(val1);
    table.insert(values);
    table.suspend_content();
    table.fetch_content();
    list< list < void* > >::iterator val_it= table.values.begin();
    string* first_val = (string*)val_it->front();
    assert(!first_val->compare("val2"));
    val_it->pop_front();
    int* sec_val = (int*)val_it->front();
    val_it->pop_front();
    assert((*sec_val) == 1);
    assert(val_it->size() == 0);
}