CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I include
LDFLAGS := -L src/lib
LDLIBS := -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lopengl32

SRC := $(wildcard src/*.cpp) src/glad.c
OBJ := $(SRC:.cpp=.o)
OUT := bin/ecs.exe

.PHONY: all clean

all: $(OUT)

$(OUT): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(OUT) $(LDFLAGS) $(LDLIBS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OUT) $(OBJ)
