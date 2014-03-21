dbeesql
=======

A db server written in cpp

Currently only create table, describe table, drop table is implemented
types are not implemented yet, they are only strings that are shown to user

To try:

install boost libraries

make

./dbeesql 8080

echo "create table hede(name text, num int);"| nc localhost 8080

echo "describe table hede;"| nc localhost 8080

echo -e "insert into hede values(\"some value\", 1);"| nc localhost 8080

echo "select * from hede;"| nc localhost 8080

echo -e "update hede set name = \"deneme1\" where name = \"some value\";"| nc localhost 8080

echo -e "select * from hede where name = \"deneme1\";"| nc localhost 8080

echo -e "select name from hede where name = \"deneme1\";"| nc localhost 8080

echo -e "remove from hede where name = \"deneme1\";"| nc localhost 8080

echo "drop table hede;"| nc localhost 8080

