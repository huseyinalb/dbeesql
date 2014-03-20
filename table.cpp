
#include "table.h"

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
    size_t values_count = 0;
    of2.write((char*)&values_count, sizeof(size_t));
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
                #ifdef DEBUG
                cout << "suspend col type: text, val: " << *val << " len: " << first_len << endl;
                #endif
                of2.write((char*)&first_len, sizeof(size_t));
                of2.write(val->c_str(), sizeof(char)*first_len);
            } else if (col_iter->second == INT_TYPE) {
                int *val = (int *)(*val_iter);
                #ifdef DEBUG
                cout << "suspend col type: int, val: " << *val << endl;
                #endif
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
    this->values.clear();
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

void Table::insert(list<string*>* values) {
    ColumnListType::iterator col_iter = this->column_list.begin();
    list<string*>::iterator val_iter = values->begin();
    list<void*> row;
    while(col_iter != this->column_list.end() && val_iter != values->end()) {
        string *val = (*val_iter);
        if (col_iter->second == TEXT_TYPE) {
            #ifdef DEBUG
            cout << "insert col type: text, val: " << *val << endl;
            #endif
            row.push_back(new string(*val));
        } else if (col_iter->second == INT_TYPE) {
            #ifdef DEBUG
            cout << "insert col type: int, val: " << *val << endl;
            #endif
            int num = atoi((*val).c_str());
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

string Table::print_rows(list< list< void*> > rows) {
    stringstream response;
    ColumnListType::iterator col_iter = this->column_list.begin();
    while(col_iter != this->column_list.end()) {
        response << col_iter->first << " : ";
        col_iter++;
    }
    response << endl;
    list< list< void* > >::iterator rows_iter = rows.begin();
    while (rows_iter != rows.end()) {
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

list< list<void*> > Table::filter(list<Condition*> *conditions) {
    list<Condition> cond_it;
    list< list< void* > >::iterator rows_iter = this->values.begin();
    list< list<void*> > filtered_rows;
    ColumnListType::iterator col_iter;
    while (rows_iter != this->values.end()) {
        bool row_valid = true;
        list<Condition*>::iterator cond_iter = conditions->begin();
        while (cond_iter != conditions->end()) {
            col_iter = this->column_list.begin();
            Condition *condition = (*cond_iter);
            list< void* >::iterator val_iter = rows_iter->begin();
            while(col_iter != this->column_list.end() && val_iter != rows_iter->end()) {
                if (!condition->column_name.compare(col_iter->first)) {
                    if ((*cond_iter)->condition == EQUALS) {
                        #ifdef DEBUG
                        cout << "EQUALS " << endl;
                        #endif
                        if (col_iter->second == TEXT_TYPE) {
                            string *val = (string*)(*val_iter);
                            #ifdef DEBUG
                            cout << "checking " << *val << " EQUALS " << condition->value << endl;
                            cout << !val->compare(condition->value) << endl;
                            #endif
                            row_valid &= !val->compare(condition->value);
                        } else if (col_iter->second == INT_TYPE) {
                            int *val = (int*)(*val_iter);
                            #ifdef DEBUG
                            cout << "checking " << *val << " EQUALS " << condition->value << endl;
                            cout << ((*val) == atoi(condition->value.c_str())) << endl;
                            #endif DEBUG
                            row_valid &= (*val) == atoi(condition->value.c_str());
                        } else
                            throw "Memory leak";
                    }
                }
                col_iter++;
                val_iter++;
            }
            cond_iter++;
        }
        if (row_valid) {
            filtered_rows.push_back(*rows_iter);
        }
    rows_iter++;
    }
    return filtered_rows;
}

string Table::update(list<Condition*> *conditions, list<SetAction*> *setActions) {
    int update_count = 0;
    stringstream response;
    list<Condition> cond_it;
    list< list< void* > >::iterator rows_iter = this->values.begin();
    ColumnListType::iterator col_iter;
    while (rows_iter != this->values.end()) {
        bool row_valid = true;
        list<Condition* >::iterator cond_iter = conditions->begin();
        while (cond_iter != conditions->end()) {
            col_iter = this->column_list.begin();
            Condition * condition = (*cond_iter);
            list< void* >::iterator val_iter = rows_iter->begin();
            while(col_iter != this->column_list.end() && val_iter != rows_iter->end()) {
                if (!condition->column_name.compare(col_iter->first)) {
                    if ((*cond_iter)->condition == EQUALS) {
                        #ifdef DEBUG
                        cout << "EQUALS " << endl;
                        #endif
                        if (col_iter->second == TEXT_TYPE) {
                            string *val = (string*)(*val_iter);
                            #ifdef DEBUG
                            cout << "checking " << *val << " EQUALS " << condition->value << endl;
                            cout << !val->compare(condition->value) << endl;
                            #endif
                            row_valid &= !val->compare(condition->value);
                        } else if (col_iter->second == INT_TYPE) {
                            int *val = (int*)(*val_iter);
                            #ifdef DEBUG
                            cout << "checking " << *val << " EQUALS " << condition->value << endl;
                            cout << ((*val) == atoi(condition->value.c_str())) << endl;
                            #endif
                            row_valid &= (*val) == atoi(condition->value.c_str());
                        } else
                            throw "type not defined";
                        update_count++;
                    }
                }
                col_iter++;
                val_iter++;
            }
            cond_iter++;
        }
        if (row_valid) {
            list<SetAction*>::iterator action_iter = setActions->begin();
            while (action_iter != setActions->end()) {
                col_iter = this->column_list.begin();
                SetAction *setAction = (*action_iter);
                list< void* >::iterator val_iter = rows_iter->begin();
                while(col_iter != this->column_list.end() && val_iter != rows_iter->end()) {
                    if (!setAction->column_name.compare(col_iter->first)) {
                        if (col_iter->second == TEXT_TYPE) {
                            string *val = (string*)(*val_iter);
                            #ifdef DEBUG
                            cout << "setting " << *val << " to " << setAction->value << endl;
                            #endif
                            delete val;
                            (*val_iter) = new string(setAction->value);
                        } else if (col_iter->second == INT_TYPE) {
                            int *val = (int*)(*val_iter);
                            #ifdef DEBUG
                            cout << "setting " << *val << " to " << setAction->value << endl;
                            #endif
                            delete val;
                            (*val_iter) = new int(atoi(setAction->value.c_str()));
                        } else
                            throw "Memory leak";
                    }
                    col_iter++;
                    val_iter++;
                }
                action_iter++;
            }
        }
        rows_iter++;
    }
    return response.str();
}

int Table::drop() {
    // TODO error detection
    int first = remove((this->table_name+".clm").c_str());
    return remove((this->table_name+".cnt").c_str()) * first;
}

Table::~Table() {
    #ifdef DEBUG
    cout << "destroying table" << this->table_name << endl;
    #endif
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
