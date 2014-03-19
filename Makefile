cc= g++
SRC_FILES= parser.cpp table.cpp lockmap.cpp
TEST_SRC_FILES= test_create.cpp test_select.cpp test_table.cpp
TEST_EXECS= $(TEST_SRC_FILES:%.cpp=%.out)
OBJECTS= $(SRC_FILES:%.cpp=%.o)
FLAGS= -lboost_thread-mt -lboost_system-mt -lboost_regex-mt -gstabs

all: $(OBJECTS)
	$(cc) main.cpp $(OBJECTS) $(FLAGS) -o dbeesql
	$(foreach file, $(TEST_SRC_FILES), $(cc) $(file) $(FLAGS) $(OBJECTS) -o $(file:%.cpp=%.out);)
objects:
	$(cc) -c $(FLAGS) $(SRC_FILES)
clean:
	rm -f *.o *.out dbeesql
test: all
	$(foreach file, $(TEST_SRC_FILES), ./$(file:%.cpp=%.out);)
