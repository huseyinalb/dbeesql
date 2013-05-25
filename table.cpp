
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
            cout << first_len << endl;
            char column_name[first_len];
            int column_type;
            in.read(column_name, sizeof(char)*first_len);
            column_name[first_len] = '\0';
            in.read((char *)&column_type, sizeof(int));
            pair<std::string, int> column(column_name, column_type);
            this->column_list.push_front(column);
        }
        in.close();
};

Table::Table(string table_name, ColumnListType column_list) {
    this->table_name = table_name;
    this->column_list = column_list;
};
    // TODO exception etc
void Table::suspend() {
        ofstream of((this->table_name + ".clm").c_str());
        size_t column_count = column_list.size();
        of.write((char*)&column_count, sizeof(size_t));
        for (ColumnListType::iterator iter = this->column_list.begin();iter != this->column_list.end(); ++iter) {
            cout << "column_list" << endl << endl;
            size_t first_len = ((*iter).first.length());
            of.write((char*)&first_len, sizeof(size_t));
            of << (*iter).first.c_str();
            of.write((char*)&(*iter).second, sizeof(int));
            
        }
        of.close();
}

int Table::drop() {
    // TODO error detection
    return remove((this->table_name+".clm").c_str());
}
