CC = g++

EXE = join

SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CCFLAGS += -std=c++11 -g

.PHONY: clean

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(CCFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f $(EXE) $(OBJ)

#compile for gdb
d: CCFLAGS += -g3

d: $(EXE)

# test results
test_result: CCFLAGS += -D TEST_RESULT
test_result: ./obj/result.o
	$(CC) -o $@ $^ $(CCFLAGS)
