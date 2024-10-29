# Compiler
CXX = g++
CXXFLAGS = -std=c++20 -I/usr/include/boost -MMD -MP -g -O1

# Linker flags
LDFLAGS = -lboost_system -lboost_context

# Source directorys
SRC_DIR = src

# Build directory
BUILD_DIR = build

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cc)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.cc, $(BUILD_DIR)/%.o, $(SRCS))

# Dependency files
DEPS = $(OBJS:.o=.d)

# Target executable
TARGET = demo

# Rule to build the target executable
$(TARGET): $(OBJS)
	 $(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc | $(BUILD_DIR)
	 $(CXX) $(CXXFLAGS) -c -o $@ $<

# Ensure the build directory exists
$(BUILD_DIR):
	 mkdir -p $(BUILD_DIR)

# Include dependency files
-include $(DEPS)

# Clean rule to remove generated files
clean:
	 rm -f $(BUILD_DIR)/*.o $(TARGET) $(BUILD_DIR)/*.d
	 rmdir $(BUILD_DIR)