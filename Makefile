
#g++ -g -Wall -o bin/pwrecov src/pwrecov.cpp src/secret_sharing.cpp src/opts.cpp -I src/include/ -lcryptopp

# the compiler: g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall
CHEADER_DIR = -I src/include/
CLIBS   = -lcryptopp

# the build target executable:
TARGET = pwrecov

all: $(TARGET)

$(TARGET): src/$(TARGET).cpp src/secret_sharing.cpp
	$(CC) $(CFLAGS) -o bin/$(TARGET) src/$(TARGET).cpp src/secret_sharing.cpp src/opts.cpp $(CHEADER_DIR) $(CLIBS)

run: bin/$(TARGET)
	./bin/pwrecov $(ARGS)

clean:
	$(RM) $(TARGET)
