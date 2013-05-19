dbeesql
=======

A db server written in cpp

Currently only create table, describe table, drop table is implemented
types are not implemented yet, they are only strings that are shown to user

To try:

install boost libraries

make

./dbeesql 8080

echo "create table hede(name text, num integer);"| nc localhost 8080

echo "describe table hede;"| nc localhost 8080

echo "drop table hede;"| nc localhost 8080
