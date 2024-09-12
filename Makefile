CC = g++ -std=c++11
SRC = tok.cpp
OBJ = $(BUILD_DIR)/tok.o
LIBRARY = $(BUILD_DIR)/libtok.a
BUILD_DIR = build

all: $(BUILD_DIR) $(LIBRARY)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(LIBRARY): $(OBJ)
	ar rcs $(LIBRARY) $(OBJ)

$(OBJ): $(SRC)
	$(CC) -c $(SRC) -o $(OBJ)

clean:
	rm -f $(OBJ) $(LIBRARY)
	rm -rf $(BUILD_DIR)
