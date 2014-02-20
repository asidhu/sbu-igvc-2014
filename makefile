CC=gcc
MAKE= make
CFLAGS=-lstdc++ -lpthread  -Wall -g
INCLUDE=-Isrc/
CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

OUTPUT_FILE = robot
all: compile_modules link

obj/%.o: src/%.cpp
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

compile:
	$(CC) -c $(CPP_FILES) $(INCLUDE) $(CFLAGS) -o $(OBJ_FILES)

compile_modules:
	-cd src && $(MAKE)
link: $(OBJ_FILES)
	$(CC) -o $(OUTPUT_FILE) $(OBJ_FILES) $(CFLAGS)
clean:
	-rm obj/*.o
	-rm $(OUTPUT_FILE)
	-cd src && $(MAKE) clean