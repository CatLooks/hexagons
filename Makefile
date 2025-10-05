CC = g++

# project directories
SRC = src
BIN = bin
OBJ = obj

# list of all .cpp files
CPP_FILES = \
	$(SRC)/main.cpp

# paths to SFML folders
INC_PATH = -IC:/SFML3/include
LIB_PATH = -LC:/SFML3/lib

# libraries for linker
LIB_SF = -lsfml-audio-s -lsfml-network-s -lsfml-graphics-s -lsfml-window-s -lsfml-system-s
LIB_CC = -lopengl32 -lgdi32 -lfreetype -lwinmm
LIB_LIST = $(LIB_SF) $(LIB_CC)

# all flags
CFLAGS = -O2 -s -Wall -std=c++17 -D SFML_STATIC $(INC_PATH)
LFLAGS = -static-libstdc++ -static-libgcc $(LIB_PATH) $(LIB_LIST)

# key files
MAIN = $(SRC)/main.cpp
EXE = $(BIN)/hex.exe

# link targets
OBJ_FILES := $(patsubst $(SRC)/%, $(OBJ)/%,$(CPP_FILES:.cpp=.o))

# default rule
default: $(OBJ) $(BIN) $(EXE)

# compile object files
$(OBJ)/%.o: $(CPP_FILES)
	@echo $^
	@echo Compiling ^<$<^> into ^<$@^>.
	$(CC) $< -o $@ -c $(CFLAGS)

# link object files
$(EXE): $(OBJ_FILES)
	@echo Linking ^<$^^> into ^<$@^>.
	$(CC) $^ -o $@ $(LFLAGS)

# directory generator
$(OBJ):
	@mkdir $(OBJ)
$(BIN):
	@mkdir $(BIN)

# compile & run
run: default
	@echo [===]
	@$(EXE)

# clean-up for binaries
clean:
	@if exist $(OBJ) rmdir /S /Q $(OBJ)
	@if exist $(BIN) rmdir /S /Q $(BIN)

# test info
test:
	@echo .cpp: $(CPP_FILES)
	@echo .obj: $(OBJ_FILES)