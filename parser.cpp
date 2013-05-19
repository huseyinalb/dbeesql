
#include "parser.h"

int token_match(list<string>::iterator tok_iter, list<string>::iterator tok_end, string str)
{
    if (tok_iter != tok_end && (*tok_iter).compare(str) == 0)
        return 1;
    else
        return 0;
}

//TODO freed when exits?
int is_ident(list<string>::iterator tok_iter, list<string>::iterator tok_end)
{
    if (tok_iter == tok_end)
        return 0;
    else
        return 1;
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
    // TODO If already created
    std::string table_name;
    list< pair<std::string, std::string> > column_list;
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
        std::string column_type;
        do {
            tok_iter++;
            if(tok_iter != tokens.end()) {
                column_name = *tok_iter;
            } else
                break;
            tok_iter++;
            if (tok_iter != tokens.end()) {
                column_type = *tok_iter;
            } else
                break;
            tok_iter++;
            pair<std::string, std::string> column(column_name, column_type);
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
    list< pair<string, string> >::iterator col_iter = table.column_list.begin();
    string response;
    while(col_iter != table.column_list.end()) {
        response += string("colname: ") + col_iter->first + string(" coltype: ") + col_iter->second + "\n";
        col_iter++;
    }
    return response;
}
