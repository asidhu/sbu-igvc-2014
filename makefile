CC=gcc
MAKE= make
CFLAGS:=-lstdc++ -lpthread  -Wall -g
INCLUDE:=-Isrc/ -Ilib/  $(shell pkg-config --cflags opencv)
MODULES = cpuinfo network arduino imu

CPUINFOCPP := $(wildcard src/modules/cpuinfo/*.cpp)
CPUINFOOBJ := $(addprefix obj/,$(notdir $(CPUINFOCPP:.cpp=.o)))
CPUINFOPAT := src/modules/cpuinfo/%.cpp

NETWORKCPP := $(wildcard src/modules/network/*.cpp)
NETWORKOBJ := $(addprefix obj/,$(notdir $(NETWORKCPP:.cpp=.o)))
NETWORKPAT := src/modules/network/%.cpp


CAMERADIR := src/modules/camera
CAMERACPP := $(wildcard $(CAMERADIR)/*.cpp)
CAMERAOBJ := $(addprefix obj/,$(notdir $(CAMERACPP:.cpp=.o)))
CAMERAPAT := $(CAMERADIR)/%.cpp


ARDUINODIR := src/modules/arduino
ARDUINOCPP := $(wildcard $(ARDUINODIR)/*.cpp)
ARDUINOOBJ := $(addprefix obj/,$(notdir $(ARDUINOCPP:.cpp=.o)))
ARDUINOPAT := $(ARDUINODIR)/%.cpp

IMUDIR := src/modules/imu
IMUCPP := $(wildcard $(IMUDIR)/*.cpp)
IMUOBJ := $(addprefix obj/,$(notdir $(IMUCPP:.cpp=.o)))
IMUPAT := $(IMUDIR)/%.cpp

CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
LINK_FILES = obj/*.o

OUTPUT_FILE = robot
all: core compile_modules link
cv: core compile_modules camera linkcv


compile_modules: $(MODULES)

obj/%.o: src/%.cpp 
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

core: $(OBJ_FILES)

compile:
	$(CC) -c $(CPP_FILES) $(INCLUDE) $(CFLAGS) -o $(OBJ_FILES)

link:
	$(CC) -o $(OUTPUT_FILE) $(LINK_FILES) $(CFLAGS)

linkcv:
	$(CC) -o $(OUTPUT_FILE) $(LINK_FILES) $(CFLAGS) $(shell pkg-config --libs opencv)
clean:
	-rm obj/*.o
	-rm $(OUTPUT_FILE)



cpuinfo: $(CPUINFOCPP) $(CPUINFOOBJ)
obj/%.o: $(CPUINFOPAT)
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

network: $(NETWORKCPP) $(NETWORKOBJ)
obj/%.o: $(NETWORKPAT)
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

camera: $(CAMERACPP) $(CAMERAOBJ)
obj/%.o: $(CAMERAPAT)
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

arduino: $(ARDUINOCPP) $(ARDUINOOBJ)
obj/%.o: $(ARDUINOPAT)
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<


imu: $(IMUCPP) $(IMUOBJ)
obj/%.o: $(IMUPAT)
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<
