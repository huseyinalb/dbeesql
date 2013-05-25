
#include "parser.h"

int token_match(list<string>::iterator tok_iter, list<string>::iterator tok_end, string str)
{
    if (tok_iter != tok_end && (*tok_iter).compare(str) == 0)
        return 1;
    else
        return 0;
}

// TODO freed when exits?
// TODO should check if alnum etc
int is_ident(list<string>::iterator tok_iter, list<string>::iterator tok_end)
{
    if (tok_iter == tok_end)
        return 0;
    else
        return 1;
}

int token_list_match(list<string>::iterator tok_iter, list<string>::iterator tok_end, ColumnListType match_list)
{
    for (ColumnListType::iterator match_iter = match_list.begin(); match_iter != match_list.end(); match_iter++) {
        if(tok_iter != tok_end && (*tok_iter).compare((*match_iter).first) == 0)
            return (*match_iter).second;
    }
    return 0;
}

ColumnListType get_types()
{
    static ColumnListType type_list;
    if (type_list.size() == 0) {
        type_list.push_front(pair<string, int>("int", INT_TYPE));
        type_list.push_front(pair<string, int>("text", TEXT_TYPE));
    }
    return type_list;
}

int get_type(list<string>::iterator tok_iter, list<string>::iterator tok_end)
{

    token_list_match(tok_iter, tok_end,
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
    boost::regex patterns("([A-Za-z]+|\\s+|,|\\(|\\)|;)");
    boost::match_results<std::string::const_iterator> what;
    boost::match_flag_type flags = boost::match_default;
    std::string::const_iterator start = str.begin();
    list<string> tokens;
    while(boost::regex_search(start, str.end(), what, patterns, flags)) {
        boost::match_results<std::string::const_iterator> results;
        if (!boost::regex_match(string(what[1]), results, boost::regex("\\s+"))) {
            cout << ":" << (what[1]) << ":"<< endl;
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
                break;
            tok_iter++;
            pair<std::string, int> column(column_name, column_type);
            cout << column_name << ":" << column_type << endl;
            column_list.push_front(column);
            if (is_cphar(tok_iter, tokens.end()))
                break;
        } while (is_comma(tok_iter, tokens.end()));
        tok_iter++;
        if (!is_semicolon(tok_iter, tokens.end()))
            throw "semicolon";
    } catch (const char* message) {
        cout << "hata ulan" << message << endl;
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

Table parse_drop(list<string> tokens)
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
        cout << "hata ulan" << message << endl;
        throw "could not parse";
    }
    Table table(table_name);
    return table;
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
        cout << "hata ulan" << message << endl;
        throw "could not parse";
    }
    Table table(table_name);
    return table;
}

string run_describe(Table table)
{
    ColumnListType::iterator col_iter = table.column_list.begin();
    stringstream response;
    while(col_iter != table.column_list.end()) {
        response << "colname: " << col_iter->first << " coltype: " << col_iter->second << endl;
        col_iter++;
    }
    return response.str();
}
