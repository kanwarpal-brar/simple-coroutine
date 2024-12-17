# Compiler
CXX = g++
CXXFLAGS = -std=c++20 -I/usr/include/boost -MMD -MP -g -DCORJOB_DEBUG

# Linker flags
LDFLAGS = -lboost_system -lboost_context

# Source directories
SRC_DIR = src
BENCHMARK_DIR = benchmark
BENCHMARK_EXEC_DIR = $(BENCHMARK_DIR)/executables

# Build directory
BUILD_DIR = build

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cc)
BENCHMARK_SRCS = $(wildcard $(BENCHMARK_DIR)/*.cc)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.cc, $(BUILD_DIR)/%.o, $(SRCS))
BENCHMARK_OBJS = $(patsubst $(BENCHMARK_DIR)/%.cc, $(BUILD_DIR)/%.o, $(BENCHMARK_SRCS))

# Dependency files
DEPS = $(OBJS:.o=.d) $(BENCHMARK_OBJS:.o=.d)

# Target executable
TARGET = demo
BENCHMARK_TARGETS = $(patsubst $(BENCHMARK_DIR)/%.cc, $(BENCHMARK_EXEC_DIR)/%, $(BENCHMARK_SRCS))

# Rule to build the target executable
$(TARGET): $(OBJS)
	 $(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to build benchmark executables
$(BENCHMARK_EXEC_DIR)/%: $(BUILD_DIR)/%.o | $(BENCHMARK_EXEC_DIR)
	 $(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

# Rule to compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc | $(BUILD_DIR)
	 $(CXX) $(CXXFLAGS) -c -o $@ $<

# Rule to compile benchmark source files into object files
$(BUILD_DIR)/%.o: $(BENCHMARK_DIR)/%.cc | $(BUILD_DIR)
	 $(CXX) $(CXXFLAGS) -c -o $@ $<

# Ensure the build and benchmark executable directories exist
$(BUILD_DIR):
	 mkdir -p $(BUILD_DIR)

$(BENCHMARK_EXEC_DIR):
	 mkdir -p $(BENCHMARK_EXEC_DIR)

# Include dependency files
-include $(DEPS)

# Clean rule to remove generated files
clean:
	 rm -f $(BUILD_DIR)/*.o $(TARGET) $(BENCHMARK_TARGETS) $(BUILD_DIR)/*.d
	 rmdir $(BUILD_DIR) $(BENCHMARK_EXEC_DIR)

# Default rule to build all targets
all: $(TARGET) $(BENCHMARK_TARGETS)

# Rule to build only benchmark targets
benchmark: $(BENCHMARK_TARGETS)

# Rule to build and run benchmarks
run-benchmark:
	$(MAKE) benchmark
	./benchmark/run.sh