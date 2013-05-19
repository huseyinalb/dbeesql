
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
            size_t second_len;
            in.read((char*)&first_len, sizeof(size_t));
            in.read((char*)&second_len, sizeof(size_t));
            cout << first_len << endl;
            cout << second_len << endl;
            char column_name[first_len], column_type[second_len];
            in.read(column_name, sizeof(char)*first_len);
            column_name[first_len] = '\0';
            in.read(column_type, sizeof(char)*second_len);
            column_type[second_len] = '\0';
            pair<std::string, std::string> column(column_name, column_type);
            column_list.push_front(column);
        }
        in.close();
};

Table::Table(string table_name, list< pair<std::string, std::string> > column_list) {
    this->table_name = table_name;
    this->column_list = column_list;
};
    // TODO exception etc
void Table::suspend() {
        ofstream of((this->table_name + ".clm").c_str());
        size_t column_count = column_list.size();
        of.write((char*)&column_count, sizeof(size_t));
        typedef pair<std::string, std::string> columntype;
        for (list< pair<std::string, std::string> >::iterator iter = this->column_list.begin();iter != this->column_list.end(); ++iter) {
            cout << "column_list" << endl << endl;
            size_t first_len = ((*iter).first.length());
            size_t second_len = ((*iter).second.length());
            of.write((char*)&first_len, sizeof(size_t));
            of.write((char*)&second_len, sizeof(size_t));
            of << (*iter).first.c_str();
            of << (*iter).second.c_str();
            
        }
        of.close();
}

int Table::drop() {
    // TODO error detection
    return remove((this->table_name+".clm").c_str());
}
