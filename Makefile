# compiler
CC = g++

# compiler flags
CFLAGS = -I utf8proc

# linker flags
LDFLAGS = -L utf8proc/build -l utf8proc

# source files
SRCS = test.cpp tokenizer.cpp

# object files (same names as sources but with .o extension)
OBJS = $(SRCS:.cpp=.o)

# executable name
TARGET = test

# directories
BUILD_DIR = utf8proc/build

# default rule to build the target executable
all: build_utf8proc $(TARGET)

# rule to build the utf8proc library
build_utf8proc:
	mkdir -p $(BUILD_DIR)
	cd utf8proc && mkdir -p build && cd build && cmake .. && make

# rule to build the target
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# rule to compile source files into object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# rule to clean up the build (remove object files and the executable)
clean:
	rm $(OBJS) $(TARGET)
	rm -rf $(BUILD_DIR)
