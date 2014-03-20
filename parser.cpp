
#include "parser.h"
#include <stdlib.h>

int token_match(list<string>::iterator tok_iter, list<string>::iterator tok_end, string str)
{
    if (tok_iter != tok_end && (*tok_iter).compare(str) == 0)
        return 1;
    else {
        #ifdef DEBUG
        if (tok_iter != tok_end) {
            cout << "expected: " << str << endl;
            cout << "got: " << *tok_iter << endl;
        }
        #endif
        return 0;
    }
}

// tokenizer checks the patterns, so no need to check all of the characters
int is_ident(list<string>::iterator tok_iter, list<string>::iterator tok_end)
{
    if (tok_iter != tok_end && (*tok_iter).size() > 0 && isalpha((*tok_iter)[0]))
        return 1;
    else {
        #ifdef DEBUG
        if (tok_iter != tok_end) {
            cout << "expected: identifier"<< endl;
            cout << "got: " << *tok_iter << endl;
        }
        #endif
        return 0;
    }
}

int is_num(list<string>::iterator tok_iter, list<string>::iterator tok_end) {
    if (tok_iter != tok_end && (*tok_iter).size() > 0 && isnumber((*tok_iter)[0]))
        return 1;
    else {
        #ifdef DEBUG
        if (tok_iter != tok_end) {
            cout << "expected: num"<< endl;
            cout << "got: " << *tok_iter << endl;
        }
        #endif
        return 0;
    }
}

int token_list_match(list<string>::iterator tok_iter, list<string>::iterator tok_end, ColumnListType match_list)
{
    for (ColumnListType::iterator match_iter = match_list.begin(); match_iter != match_list.end(); match_iter++) {
        if(tok_iter != tok_end && (*tok_iter).compare((*match_iter).first) == 0) {
            return (*match_iter).second;
        }
    }
    return 0;
}

ColumnListType get_types()
{
    static ColumnListType type_list;
    if (type_list.size() == 0) {
        type_list.push_back(pair<string, int>("int", INT_TYPE));
        type_list.push_back(pair<string, int>("text", TEXT_TYPE));
    }
    return type_list;
}

int is_text(list<string>::iterator tok_iter, list<string>::iterator tok_end) {
    if (tok_iter != tok_end && (*tok_iter).size() > 1 && (*tok_iter)[0] == '\"')
        return 1;
    else {
        #ifdef DEBUG
        if (tok_iter != tok_end) {
            cout << "expected: text" << endl;
            cout << "got: " << *tok_iter << endl;
        }
        #endif
        return 0;
    }
}

int get_type(list<string>::iterator tok_iter, list<string>::iterator tok_end)
{
    return token_list_match(tok_iter, tok_end,
                      get_types());
}

int is_ophar(list<string>::iterator tok_iter, list<string>::iterator tok_end)
{
    return token_match(tok_iter, tok_end, string("("));
}

int is_cphar(list<string>::iterator tok_iter, list<string>::iterator tok_end)
{
    return token_match(tok_iter, tok_end, string(")"));
}

int is_condition(list<string>::iterator tok_iter, list<string>::iterator tok_end)
{
    return token_match(tok_iter, tok_end, string("="))
    || token_match(tok_iter, tok_end, string(">"))
    || token_match(tok_iter, tok_end, string("<"));
}

int is_comma(list<string>::iterator tok_iter, list<string>::iterator tok_end)
{
    return token_match(tok_iter, tok_end, string(","));
}

int is_semicolon(list<string>::iterator tok_iter, list<string>::iterator tok_end)
{
    return token_match(tok_iter, tok_end, string(";"));
}

list<string> tokenize(const string str)
{
    // TODO will be refactored
    boost::regex patterns("([A-Za-z_][A-Za-z0-9_]+|[0-9]+|\\s+|,|\\(|\\)|;|\\*|=|\\>|\\<|\\\"[^\\\"]*\\\")");
    boost::match_results<std::string::const_iterator> what;
    boost::match_flag_type flags = boost::match_default;
    std::string::const_iterator start = str.begin();
    list<string> tokens;
    while(boost::regex_search(start, str.end(), what, patterns, flags)) {
        boost::match_results<std::string::const_iterator> results;
        if (!boost::regex_match(string(what[1]), results, boost::regex("\\s+"))) {
            #ifdef DEBUG
            cout << ":" << (what[1]) << ":"<< endl;
            #endif
            tokens.push_back(what[0]);
        }
        start = what[0].second;
    }
    return tokens;
}

int is_create(list<string> tokens)
{
    list<string>::iterator tok_iter = tokens.begin();
    if (!token_match(tok_iter, tokens.end(), "create"))
        return 0;
    tok_iter++;
    if (!token_match(tok_iter, tokens.end(), "table"))
        return 0;
    tok_iter++;
    return 1;
}

int is_insert(list<string> tokens)
{
    list<string>::iterator tok_iter = tokens.begin();
    if (!token_match(tok_iter, tokens.end(), "insert"))
        return 0;
    tok_iter++;
    if (!token_match(tok_iter, tokens.end(), "into"))
        return 0;
    tok_iter++;
    return 1;
}

list<Condition*>* parse_where(list<string>::iterator& tok_iter, list<string>::iterator tok_end) {
    list<Condition*>* conditions = new list<Condition*>;
    if (token_match(tok_iter, tok_end, "where")) {
        do {
            tok_iter++;
            Condition *condition = new Condition;
            if(is_ident(tok_iter, tok_end)) {
                condition->column_name = *tok_iter;
            } else
                throw "waiting for a column name";
            #ifdef DEBUG
            cout << condition->column_name << endl;
            #endif
            tok_iter++;
            if(is_condition(tok_iter, tok_end)) {
                if (*tok_iter == "=")
                    condition->condition = EQUALS;
                else if (*tok_iter == ">")
                    condition->condition = BIGGER;
                else if (*tok_iter == "<")
                    condition->condition = SMALLER;
            } else
                throw "waiting for a condition";
            tok_iter++;
            if(is_text(tok_iter, tok_end)) {
                condition->value = (*tok_iter).substr(1, (*tok_iter).size()-2);
            } else if (is_num(tok_iter, tok_end)){
                condition->value = *tok_iter;
            } else {
                break;
            }
            #ifdef DEBUG
            cout << condition->column_name << endl;
            #endif
            tok_iter++;
            conditions->push_back(condition);
        } while (is_comma(tok_iter, tok_end));
    }
    return conditions;
}

Query* parse_insert(list<string> tokens)
{
    // ignores if column_type and column_name is given wrong, should be fixed
    // TODO If already created
    std::string table_name;
    list<string>::iterator tok_iter = tokens.begin();
    list<string*> *insert_values = new list<string*>;
    list<string> column_names;
    try {
        // is_insert
        tok_iter++;
        tok_iter++;
        if (!is_ident(tok_iter, tokens.end()))
            throw "ident";
        table_name = *tok_iter;
        tok_iter++;
        if (!token_match(tok_iter, tokens.end(), "values"))
            throw "values";
        tok_iter++;
        if (!is_ophar(tok_iter, tokens.end()))
            throw "ophar";
        // TODO quoted value
        do {
            tok_iter++;
            string *column_value;
            if(is_text(tok_iter, tokens.end())) {
                column_value = new string((*tok_iter).substr(1, (*tok_iter).size()-2));
            } else if (is_num(tok_iter, tokens.end())){
                column_value = new string(*tok_iter);
            } else {
                break;
            }
            insert_values->push_back(column_value);
            tok_iter++;
        } while (is_comma(tok_iter, tokens.end()));
        if (!is_cphar(tok_iter, tokens.end()))
            throw "cphar";
        tok_iter++;
        if (!is_semicolon(tok_iter, tokens.end()))
            throw "semicolon";
    } catch (const char* message) {
        #ifdef DEBUG
        cout << "error:" << message << endl;
        #endif
        throw "could not parse";
    }
    return new Query(table_name, insert_values);
}

int is_select(list<string> tokens)
{
    list<string>::iterator tok_iter = tokens.begin();
    if (!token_match(tok_iter, tokens.end(), "select"))
        return 0;
    return 1;
}

Query *parse_select(list<string> tokens) {
    std::string table_name;
    ColumnListType column_list;
    list<Condition*> *conditions = new list<Condition*>;
    list<string>::iterator tok_iter = tokens.begin();
    // is_select
    tok_iter++;
    if (!token_match(tok_iter, tokens.end(), "*"))
        throw("not select *");
    tok_iter++;
    if (!token_match(tok_iter, tokens.end(), "from"))
        throw("waiting for from");
    tok_iter++;
    if (!is_ident(tok_iter, tokens.end()))
        throw("no table name");
    table_name = *tok_iter;
    tok_iter++;
    conditions = parse_where(tok_iter, tokens.end());
    if(!is_semicolon(tok_iter, tokens.end()))
       throw("no semicolumn");
    return new Query(table_name, conditions, (list<SetAction*>*)0);
}

Table parse_create(list<string> tokens)
{
    // ignores if column_type and column_name is given wrong, should be fixed
    // TODO If already created
    std::string table_name;
    ColumnListType column_list;
    list<string>::iterator tok_iter = tokens.begin();
    try {
        // is_create
        tok_iter++;
        tok_iter++;
        if (!is_ident(tok_iter, tokens.end()))
            throw "ident";
        table_name = *tok_iter;
        tok_iter++;
        if (!is_ophar(tok_iter, tokens.end()))
            throw "ophar";
        std::string column_name;
        int column_type;
        do {
            tok_iter++;
            if(is_ident(tok_iter, tokens.end())) {
                column_name = *tok_iter;
            } else
                break;
            tok_iter++;
            column_type = get_type(tok_iter, tokens.end());
            if (column_type == 0)
                throw "unidentified type";
            tok_iter++;
            pair<std::string, int> column(column_name, column_type);
            #ifdef DEBUG
            cout << column_name << ":" << column_type << endl;
            #endif
            column_list.push_back(column);
        } while (is_comma(tok_iter, tokens.end()));
        if (!is_cphar(tok_iter, tokens.end()))
            throw "close parentheses";
        tok_iter++;
        if (!is_semicolon(tok_iter, tokens.end()))
            throw "semicolon";
    } catch (const char* message) {
        #ifdef DEBUG
        cout << *tok_iter;
        cout << "hata ulan " << message << endl;
        #endif
        throw "could not parse";
    }
    Table table(table_name, column_list);
    return table;
}

int is_drop(list<string> tokens)
{
    list<string>::iterator tok_iter = tokens.begin();
    if (!token_match(tok_iter, tokens.end(), "drop"))
        return 0;
    tok_iter++;
    if (!token_match(tok_iter, tokens.end(), "table"))
        return 0;
    tok_iter++;
    return 1;
}

string parse_drop(list<string> tokens)
{
    std::string table_name;
    list<string>::iterator tok_iter = tokens.begin();
    try {
        // is_create
        tok_iter++;
        tok_iter++;
        if (!is_ident(tok_iter, tokens.end()))
            throw "ident";
        table_name = *tok_iter;
        tok_iter++;
        if (!is_semicolon(tok_iter, tokens.end()))
            throw "semicolon";
    } catch (const char* message) {
        #ifdef DEBUG
        cout << "hata ulan" << message << endl;
        #endif
        throw "could not parse";
    }
    return table_name;
}

int is_remove(list<string> tokens) {
    list<string>::iterator tok_iter = tokens.begin();
    if (!token_match(tok_iter, tokens.end(), "remove"))
        return 0;
    return 1;
}

Query *parse_remove(list<string> tokens) {
    std::string table_name;
    ColumnListType column_list;
    list<Condition*> *conditions = new list<Condition*>;
    list<string>::iterator tok_iter = tokens.begin();
    // is_remove
    tok_iter++;
    if (!token_match(tok_iter, tokens.end(), "from"))
        throw("waiting for from");
    tok_iter++;
    if (!is_ident(tok_iter, tokens.end()))
        throw("no table name");
    table_name = *tok_iter;
    tok_iter++;
    conditions = parse_where(tok_iter, tokens.end());
    if(!is_semicolon(tok_iter, tokens.end()))
        throw("no semicolumn");
    return new Query(table_name, conditions, (list<SetAction*>*)0);
}



int is_describe(list<string> tokens)
{
    list<string>::iterator tok_iter = tokens.begin();
    if (!token_match(tok_iter, tokens.end(), "describe"))
        return 0;
    tok_iter++;
    if (!token_match(tok_iter, tokens.end(), "table"))
        return 0;
    tok_iter++;
    return 1;
}

Table parse_describe(list<string> tokens)
{
    std::string table_name;
    list<string>::iterator tok_iter = tokens.begin();
    try {
        // is_create
        tok_iter++;
        tok_iter++;
        if (!is_ident(tok_iter, tokens.end()))
            throw "ident";
        table_name = *tok_iter;
        tok_iter++;
        if (!is_semicolon(tok_iter, tokens.end()))
            throw "semicolon";
    } catch (const char* message) {
        #ifdef DEBUG
        cout << "hata ulan" << message << endl;
        #endif
        throw "could not parse";
    }
    Table table(table_name);
    return table;
}

int is_update(list<string> tokens) {
    list<string>::iterator tok_iter = tokens.begin();
    if (!token_match(tok_iter, tokens.end(), "update"))
        return 0;
    return 1;
}

Query *parse_update(list<string> tokens) {
    std::string table_name;
    ColumnListType column_list;
    list<Condition*> *conditions = new list<Condition *>;
    list<SetAction*> *setActions = new list<SetAction *>;
    list<string>::iterator tok_iter = tokens.begin();
    // is_update
    tok_iter++;
    if (!is_ident(tok_iter, tokens.end()))
        throw("no table name");
    table_name = *tok_iter;
    tok_iter++;
    if (!token_match(tok_iter, tokens.end(), "set"))
        throw "set keyword expected";
    do {
        tok_iter++;
        SetAction *setAction = new SetAction;
        if(is_ident(tok_iter, tokens.end())) {
            setAction->column_name = *tok_iter;
        } else
            throw "waiting for a column name";
        tok_iter++;
        if(!token_match(tok_iter, tokens.end(), "="))
            throw "waiting for \"=\"";
        tok_iter++;
        if(is_text(tok_iter, tokens.end())) {
            setAction->value = (*tok_iter).substr(1, (*tok_iter).size()-2);
        } else if (is_num(tok_iter, tokens.end())){
            setAction->value = *tok_iter;
        } else {
            break;
        }
        tok_iter++;
        setActions->push_back(setAction);
    } while (is_comma(tok_iter, tokens.end()));
    
    conditions = parse_where(tok_iter, tokens.end());
    if(!is_semicolon(tok_iter, tokens.end()))
        throw("no semicolumn");
    Table *table = new Table(table_name);
    return new Query(table_name, conditions, setActions);
}

string run_remove(Table& table, Query* query)
{
    table.fetch_content();
    string response = table.process_according_rows(query, remove_row);
    table.suspend_content();
    return response;
}

string run_update(Table& table, Query* query)
{
    table.fetch_content();
    string response = table.process_according_rows(query, update_row);
    table.suspend_content();
    return response;
}

string run_describe(Table& table)
{
    return table.describe();
}
       
string run_insert(Table& table, Query *query)
{
    stringstream response;
    table.fetch_content();
    table.insert(query->insert_values);
    table.suspend_content();
    return response.str();
}

string run_select(Table& table, Query* query)
{
    table.fetch_content();
    list< list<void*> > rows = table.filter(query->conditions);
    return table.print_rows(rows);
}
