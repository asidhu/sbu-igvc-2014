CC=gcc
MAKE= make
CFLAGS=-lstdc++ -lpthread  -Wall -g
INCLUDE=-I../../../src/
CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(addprefix ../../../obj/,$(notdir $(CPP_FILES:.cpp=.o)))

OUTPUT_FILE = robot
all: compile

obj/%.o: src/%.cpp
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

compile:
	$(CC) -c $(CPP_FILES) $(INCLUDE) $(CFLAGS) -o $(OBJ_FILES)

clean:
