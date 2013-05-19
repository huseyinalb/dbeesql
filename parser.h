
#ifndef __dbeesql__parser__
#define __dbeesql__parser__

#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <list>
#include <string>
#include "table.h"

using namespace std;
using namespace boost;
list<string> tokenize(const string str);
int is_create(list<string> tokens);
Table parse_create(list<string> tokens);
int is_drop(list<string> tokens);
Table parse_drop(list<string> tokens);
int is_describe(list<string> tokens);
Table parse_describe(list<string> tokens);
void run_describe(Table table);

#endif /* defined(__dbeesql__parser__) */
