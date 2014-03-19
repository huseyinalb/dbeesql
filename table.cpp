
#include "table.h"
// TODO DESTROY!!!
Table::Table(string tablename)
    {
        this->table_name = tablename;
        ifstream in((tablename + ".clm").c_str());
        if (!in)
            throw "table does not exist";
        size_t column_count;
        in.read((char*)&column_count, sizeof(size_t));
        for (int i=0; i<column_count; i++){
            size_t first_len;
            in.read((char*)&first_len, sizeof(size_t));
            cout << first_len << endl;
            char column_name[first_len+1];
            int column_type;
            in.read(column_name, sizeof(char)*first_len);
            column_name[first_len] = '\0';
            in.read((char *)&column_type, sizeof(int));
            pair<std::string, int> column(column_name, column_type);
            this->column_list.push_back(column);
        }
        in.close();
};

Table::Table(string table_name, ColumnListType column_list) {
    this->table_name = table_name;
    this->column_list = column_list;
};
    // TODO exception etc
void Table::create() {
    ofstream of((this->table_name + ".clm").c_str());
    size_t column_count = column_list.size();
    of.write((char*)&column_count, sizeof(size_t));
    for (ColumnListType::iterator iter = this->column_list.begin();iter != this->column_list.end(); ++iter) {
        size_t first_len = ((*iter).first.length());
        of.write((char*)&first_len, sizeof(size_t));
        of << (*iter).first.c_str();
        of.write((char*)&(*iter).second, sizeof(int));
    }
    of.close();
    // opening content file
    ofstream of2((this->table_name + ".cnt").c_str());
    of2.close();
}

void Table::suspend_content() {
    
    // opening content file
    ofstream of2((this->table_name + ".cnt").c_str());
    size_t values_count = values.size();
    of2.write((char*)&values_count, sizeof(size_t));
    list< list< void* > >::iterator row_iter;
    list< void* >::iterator val_iter;
    ColumnListType::iterator col_iter;
    for (row_iter = this->values.begin(); row_iter != this->values.end(); row_iter++) {
        for (val_iter = row_iter->begin(),
                 col_iter = this->column_list.begin();
             col_iter != this->column_list.end() && val_iter != row_iter->end(); col_iter++, val_iter++) {
            if (col_iter->second == TEXT_TYPE) {
                string *val = (string *)(*val_iter);
                size_t first_len = (*val).length();
                cout << "suspend col type: text, val: " << *val << " len: " << first_len << endl;
                of2.write((char*)&first_len, sizeof(size_t));
                of2.write(val->c_str(), sizeof(char)*first_len);
            } else if (col_iter->second == INT_TYPE) {
                int *val = (int *)(*val_iter);
                cout << "suspend col type: int, val: " << *val << endl;
                of2.write((char*)val, sizeof(int));
            }
        }
    }
    of2.close();
}

void Table::fetch_content() {
    // opening content file
    ifstream if2((this->table_name + ".cnt").c_str());
    if (!if2)
         throw "table does not exist";
    size_t values_count;
    ColumnListType::iterator col_iter;
    if2.read((char*)&values_count, sizeof(size_t));
    this->values.empty();
    for (int i = 0; i < values_count; i++) {
        list< void* > row;
        for (col_iter = this->column_list.begin();
             col_iter != this->column_list.end(); col_iter++) {
            size_t length;
            if (col_iter->second == TEXT_TYPE) {
                if2.read((char*)&length, sizeof(size_t));
                char val[length+1];
                if2.read((char*)val, sizeof(char)*length);
                val[length] = '\0';
                cout << val << "__ " <<  endl;
                row.push_back((new string(val)));
            } else if (col_iter->second == INT_TYPE) {
                int *val = new int;
                if2.read((char*)val, sizeof(int));
                row.push_back(val);
            }
        }
        this->values.push_back(row);
    }
    if2.close();
}

void Table::insert(list<string> values) {
    ColumnListType::iterator col_iter = this->column_list.begin();
    list<string>::iterator val_iter = values.begin();
    list<void*> row;
    while(col_iter != this->column_list.end() && val_iter != values.end()) {
        string val = (*val_iter);
        if (col_iter->second == TEXT_TYPE) {
            cout << "insert col type: text, val: " << val << endl;
            row.push_back(new string(val));
        } else if (col_iter->second == INT_TYPE) {
            // TODO non integer value
            cout << "insert col type: int, val: " << val << endl;
            int num = atoi(val.c_str());
            row.push_back(new int(num));
        } else
            throw "Type error";
        col_iter++;
        val_iter++;
    }
    if (col_iter != this->column_list.end())
        throw("not enough values");
    this->values.push_back(row);
}

string Table::select() {
    
    stringstream response;
    ColumnListType::iterator col_iter = this->column_list.begin();
    while(col_iter != this->column_list.end()) {
        response << col_iter->first << " : ";
        col_iter++;
    }
    response << endl;
    list< list< void* > >::iterator rows_iter = this->values.begin();
    while (rows_iter != this->values.end()) {
        list< void* >::iterator val_iter = rows_iter->begin();
        col_iter = this->column_list.begin();
        while(col_iter != this->column_list.end() && val_iter != rows_iter->end()) {
            if (col_iter->second == TEXT_TYPE) {
                string *val = (string*)(*val_iter);
                response << (*val) << " : ";
            } else if (col_iter->second == INT_TYPE) {
                int *val = (int*)(*val_iter);
                response << (*val) << " : ";
            } else
                return "Type error";
            col_iter++;
            val_iter++;
        }
        rows_iter++;
        response << endl;
    }
    return response.str();
}

string Table::describe() {
    ColumnListType::iterator col_iter = this->column_list.begin();
    stringstream response;
    while(col_iter != this->column_list.end()) {
        response << "colname: " << col_iter->first << " coltype: " << col_iter->second << endl;
        col_iter++;
    }
    return response.str();
}

int Table::drop() {
    // TODO error detection
    return remove((this->table_name+".clm").c_str());
    return remove((this->table_name+".cnt").c_str());
}

Table::~Table() {
    cout << "destroying table" << endl;
    list< list< void* > >::iterator rows_iter = this->values.begin();
    ColumnListType::iterator col_iter;
    while (rows_iter != this->values.end()) {
        list< void* >::iterator val_iter = rows_iter->begin();
        col_iter = this->column_list.begin();
        while(col_iter != this->column_list.end() && val_iter != rows_iter->end()) {
            if (col_iter->second == TEXT_TYPE) {
                string *val = (string*)(*val_iter);
                delete val;
            } else if (col_iter->second == INT_TYPE) {
                int *val = (int*)(*val_iter);
                delete val;
            } else
                throw "Memory leak";
            col_iter++;
            val_iter++;
        }
        rows_iter++;
    }
    
}
