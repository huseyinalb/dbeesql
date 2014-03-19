
#ifndef __dbeesql__parser__
#define __dbeesql__parser__

#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <list>
#include <string>
#include "table.h"
#include "updatequery.h"

using namespace std;
using namespace boost;

list<string> tokenize(const string str);
int is_create(list<string> tokens);
Table parse_create(list<string> tokens);
int is_drop(list<string> tokens);
Table parse_drop(list<string> tokens);
int is_describe(list<string> tokens);
pair< list<string>, string > parse_insert(list<string> tokens);
int is_insert(list<string> tokens);
pair< Table, list<Condition> > parse_select(list<string> tokens);
int is_select(list<string> tokens);
string run_select(Table table, list<Condition> conditions);
string run_insert(list<string> values, string table_name);
Table parse_describe(list<string> tokens);
string run_describe(Table table);
int is_update(list<string> tokens);
UpdateQuery parse_update(list<string> tokens);
string run_update(Table table, list<Condition> conditions, list<SetAction> setActions);

#endif /* defined(__dbeesql__parser__) */
