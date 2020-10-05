#https://gist.github.com/serialhex/ada2b37581591716d41f70edd2986938
#define macros
EXE = compress.exe
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
# add other things here...

CFLAGS = -c -Wall

all: clean create_dirs make_exe

make_exe: $(EXE)

# link the files into an executable
$(EXE): $(OBJ_FILES)
	g++ -o $(BIN_DIR)\$@ $^

# compile each .cpp file to its respective .obj file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CFLAGS) -c -o $@ $<

# create output directories
create_dirs:
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

# delete output directories
clean:
	@if exist $(BIN_DIR) rmdir /S /Q $(BIN_DIR)
	@if exist $(OBJ_DIR) rmdir /S /Q $(OBJ_DIR)