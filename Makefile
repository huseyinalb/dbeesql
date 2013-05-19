cc= g++
SRC_FILES= parser.cpp table.cpp lockmap.cpp
OBJECTS= $(SRC_FILES:%.cpp=%.o)
FLAGS= -lboost_thread-mt -lboost_system-mt -lboost_regex-mt 

all: objects
	$(cc) main.cpp $(OBJECTS) $(FLAGS) -o dbeesql
objects:
	$(cc) -c $(FLAGS) $(SRC_FILES)
clean:
	rm -f *.o *.out dbeesql
