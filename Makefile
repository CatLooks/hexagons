# Compiler
CC = g++

# Project directories
SRC = src
BIN = bin
OBJ = obj

# List of all .cpp files
CPP_FILES = $(wildcard $(SRC)/*.cpp $(SRC)/**/*.cpp)

# Detect operating system
ifeq ($(OS),Windows_NT)
    # Windows-specific settings
    INC_PATH = -IC:/SFML3/include
    LIB_PATH = -LC:/SFML3/lib
    LIB_SF = -lsfml-audio-s -lsfml-network-s -lsfml-graphics-s -lsfml-window-s -lsfml-system-s
    LIB_CC = -lopengl32 -lgdi32 -lfreetype -lwinmm
    LIB_LIST = $(LIB_SF) $(LIB_CC)
    CFLAGS = -O2 -s -Wall -std=c++17 -D SFML_STATIC $(INC_PATH)
    LFLAGS = -static-libstdc++ -static-libgcc $(LIB_PATH) $(LIB_LIST)
    MKDIR = mkdir
    RM = rmdir /S /Q
    EXECUTABLE = $(BIN)/hex.exe
else
    # Linux-specific settings
    INC_PATH = $(shell pkg-config --cflags sfml-graphics sfml-window sfml-system)
    LIB_PATH = $(shell pkg-config --libs sfml-graphics sfml-window sfml-system)
    CFLAGS = -O2 -Wall -std=c++17 $(INC_PATH)
    LFLAGS = $(LIB_PATH)
    MKDIR = mkdir -p
    RM = rm -rf
    EXECUTABLE = $(BIN)/hex
endif

# Key files
MAIN = $(SRC)/main.cpp
EXE = $(EXECUTABLE)

# Link targets
OBJ_FILES := $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(CPP_FILES))

# Default rule
default: $(OBJ) $(BIN) $(EXE)

# Compile object files
$(OBJ)/%.o: $(SRC)/%.cpp
	@echo Compiling $< into $@.
	$(CC) $< -o $@ -c $(CFLAGS)

# Link object files
$(EXE): $(OBJ_FILES)
	@echo Linking $^ into $@.
	$(CC) $^ -o $@ $(LFLAGS)

# Directory generator
$(OBJ):
	$(MKDIR) $(OBJ)
$(BIN):
	$(MKDIR) $(BIN)

# Compile & run
run: default
	@echo [===]
ifeq ($(OS),Windows_NT)
	@$(EXE)
else
	@LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH ./$(EXE)
endif

# Clean-up for binaries
clean:
	$(RM) $(OBJ) $(BIN)

# Test info
test:
	@echo .cpp: $(CPP_FILES)
	@echo .obj: $(OBJ_FILES)
