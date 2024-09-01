# Define compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I. -Itests
AR = ar
ARFLAGS = rcs
BUILD_DIR = build
LIB_DIR = $(BUILD_DIR)
LIBRARY = libproject.a

# Define source files and object files
SRC = PhysicalMemory.cpp VirtualMemory.cpp
OBJ = $(SRC:.cpp=.o)

# Define test source files and corresponding object files
TEST_SRC = tests/test1_write_read_all_virtual_memory.cpp tests/test2_write_one_page_twice_and_read.cpp
TEST_OBJ = $(TEST_SRC:.cpp=.o)

# Define SimpleTest
SIMPLE_TEST_SRC = tests/SimpleTest.cpp
SIMPLE_TEST_OBJ = $(SIMPLE_TEST_SRC:.cpp=.o)

# Create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile the library
$(LIBRARY): $(BUILD_DIR) $(OBJ)
	$(AR) $(ARFLAGS) $(LIB_DIR)/$(LIBRARY) $(OBJ)

# Compile the test source files
$(BUILD_DIR)/%.o: tests/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build the test executables
tests/test1: $(BUILD_DIR)/test1_write_read_all_virtual_memory.o $(LIBRARY)
	$(CXX) $(CXXFLAGS) $< -L$(BUILD_DIR) -lproject -o $@

tests/test2: $(BUILD_DIR)/test2_write_one_page_twice_and_read.o $(LIBRARY)
	$(CXX) $(CXXFLAGS) $< -L$(BUILD_DIR) -lproject -o $@

# Build the SimpleTest executable
simple_test: $(BUILD_DIR)/SimpleTest.o $(LIBRARY)
	$(CXX) $(CXXFLAGS) $< -L$(BUILD_DIR) -lproject -o $@

# Link all tests together
run_tests: tests/test1 tests/test2 simple_test
	./tests/test1
	./tests/test2
	./simple_test

# Clean up build artifacts
clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/$(LIBRARY) $(BUILD_DIR)/test1 $(BUILD_DIR)/test2 $(BUILD_DIR)/simple_test

.PHONY: all clean run_tests simple_test
