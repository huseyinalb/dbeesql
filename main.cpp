#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <string>
#include <list>
#include <fstream>
#include "parser.h"
#include "lockmap.h"

using namespace boost;
using namespace std;
const int max_length = 1024;
typedef shared_ptr<asio::ip::tcp::socket> socket_ptr;

string process(char* data)
{
    const std::string str(data);
    list<string> tokens = tokenize(str);
    string response;
    Query *query = (Query*)0;
    try {
        if (is_create(tokens)) {
            Table table = parse_create(tokens);
            try {
                LockMap::get_instance().lockReadLock(table.table_name);
                table.create();
                LockMap::get_instance().unlockReadLock(table.table_name);
                response = "OK\n";
            } catch (const char * message) {
                LockMap::get_instance().unlockReadLock(table.table_name);
                response = message;
                cout << message << endl;
            }
        } else if (is_drop(tokens)) {
            string table_name = parse_drop(tokens);
            try {
                LockMap::get_instance().lockReadLock(table_name);
                Table table(table_name);
                table.drop();
                LockMap::get_instance().unlockReadLock(table.table_name);
                response = "OK\n";
            } catch (const char * message) {
                LockMap::get_instance().unlockReadLock(table_name);
                response = message;
                cout << message << endl;
            }
        } else if (is_describe(tokens)) {
            Table table = parse_describe(tokens);
            try {
                LockMap::get_instance().lockWriteLock(table.table_name);
                response = run_describe(table);
                LockMap::get_instance().unlockWriteLock(table.table_name);
            } catch (const char * message) {
                LockMap::get_instance().unlockWriteLock(table.table_name);
                response = message;
                cout << message << endl;
            }
        } else if (is_insert(tokens)) {
            query = parse_insert(tokens);
            try {
                LockMap::get_instance().lockWriteLock(query->table_name);
                Table table(query->table_name);
                LockMap::get_instance().unlockWriteLock(query->table_name);
                LockMap::get_instance().lockReadLock(query->table_name);
                response = run_insert(table, query);
                LockMap::get_instance().unlockReadLock(query->table_name);
            } catch (const char * message) {
                LockMap::get_instance().unlockWriteLock(query->table_name);
                LockMap::get_instance().unlockReadLock(query->table_name);
                response = message;
                cout << message << endl;
            }
        } else if (is_select(tokens)){
            query = parse_select(tokens);
            try {
                LockMap::get_instance().lockWriteLock(query->table_name);
                Table table(query->table_name);
                response = run_select(table, query);
                LockMap::get_instance().unlockWriteLock(table.table_name);
            } catch (const char * message) {
                LockMap::get_instance().unlockWriteLock(query->table_name);
                response = message;
                cout << message << endl;
            }
        } else if (is_remove(tokens)){
            query = parse_remove(tokens);
            try {
                LockMap::get_instance().lockReadLock(query->table_name);
                Table table(query->table_name);
                response = run_remove(table, query);
                LockMap::get_instance().unlockReadLock(table.table_name);
            } catch (const char * message) {
                LockMap::get_instance().unlockReadLock(query->table_name);
                response = message;
                cout << message << endl;
            }
        } else if (is_update(tokens)){
            query = parse_update(tokens);
            try {
                LockMap::get_instance().lockWriteLock(query->table_name);
                Table table(query->table_name);
                LockMap::get_instance().unlockWriteLock(table.table_name);
                LockMap::get_instance().lockReadLock(table.table_name);
                response = run_update(table, query);
                LockMap::get_instance().unlockReadLock(table.table_name);
            } catch (const char * message) {
                LockMap::get_instance().unlockWriteLock(query->table_name);
                LockMap::get_instance().unlockReadLock(query->table_name);
                response = message;
                cout << message << endl;
            }
        } else {
            string message("command does not exist");
            response = message;
            cout << message << endl;
        }
    
    } catch (char* message) {
        response = "internal error";
        cout << "hata ulan" << message << endl;
    }
    if (query)
        delete query;
    return response;
}

void session(socket_ptr sock)
{
    try
    {
        for (;;)
        {
            char data[max_length];
            
            boost::system::error_code error;
            sock->read_some(boost::asio::buffer(data), error);
            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.
            size_t len = strlen(data);
            if (data[len-1] == '\n') {
                if (len > 1 && data[len-2] == '\r')
                    data[len-2] = '\0';
                else
                    data[len-1] = '\0';
            }
            string response = process(data);
            size_t length = response.length();
            boost::asio::write(*sock, boost::asio::buffer(response.c_str(), length));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}

void server(boost::asio::io_service& io_service, short port)
{
    asio::ip::tcp::acceptor a(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
    for (;;)
    {
        socket_ptr sock(new boost::asio::ip::tcp::socket(io_service));
        a.accept(*sock);
        boost::thread t(boost::bind(session, sock));
    }
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: dbeesql <port>\n";
            return 1;
        }
        
        boost::asio::io_service io_service;
        server(io_service, atoi(argv[1]));
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    
    return 0;
}

