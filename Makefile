# compiler
CC = g++ -std=c++11

# compiler flags
CFLAGS = -I include/utf8proc -I include/cereal/include

# linker flags
LDFLAGS = -L include/utf8proc/build -l utf8proc

# source files
SRCS = test.cpp tok.cpp

# object files (same names as sources but with .o extension)
OBJS = $(SRCS:.cpp=.o)

# executable name
TARGET = test

# directories
UTF8PROC_BUILD_DIR = include/utf8proc/build

# default rule to build the target executable
all: build_utf8proc $(TARGET)

# rule to build the utf8proc library
build_utf8proc:
	mkdir -p $(UTF8PROC_BUILD_DIR)
	cd include/utf8proc && mkdir -p build && cd build && cmake .. && make

# rule to build the target
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# rule to compile source files into object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# rule to clean up the build (remove object files and the executable)
clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(UTF8PROC_BUILD_DIR)
