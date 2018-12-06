CC = g++

EXE = join

SRC_DIR = src
OBJ_DIR = obj
TEST_SRC_DIR = tests
TEST_OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.cpp)
TEST_SRC = $(wildcard $(TEST_SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJ = $(TEST_SRC:$(TEST_SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CCFLAGS = -std=c++11

.PHONY: clean
.PHONY: directories

$(EXE): directories $(OBJ)
	$(CC) -o $@ $(OBJ) $(CCFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CCFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f $(EXE) $(OBJ) $(TEST_OBJ) test_*

directories:
	mkdir -p obj

#compile for gdb
d: CCFLAGS += -g3

d: $(EXE)

############## tests
test_result: CCFLAGS += -D TEST_RESULT
test_result: ./obj/result.o
	$(CC) -o $@ $^ $(CCFLAGS)

test_utils: ./obj/utils.o ./obj/testUtils.o
	$(CC) -o $@ $^ $(CCFLAGS)

test_parsing: ./obj/query.o ./obj/testParsing.o ./obj/utils.o
	$(CC) -o $@ $^ $(CCFLAGS)
	#
	# OK! now try ./test_parsing < workloads/small/small.work
	#
