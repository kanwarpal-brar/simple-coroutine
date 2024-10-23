# Compiler
CXX = g++
CXXFLAGS = -std=c++20 -I/usr/include/boost

# Source directory
SRC_DIR = src

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cc)

# Object files
OBJS = $(SRCS:.cc=.o)

# Target executable
TARGET = demo

# Rule to build the target executable
$(TARGET): $(OBJS)
	 $(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile source files into object files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	 $(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean rule to remove generated files
clean:
	 rm -f $(OBJS) $(TARGET)