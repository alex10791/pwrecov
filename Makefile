
#g++ -g -Wall -o bin/main src/main.cpp

# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall
CLIBDIR = -L /usr/local/Cellar/cryptopp/5.6.3_1/lib/ 
#CLIBDIR = -
CHEADER_DIR = -I /usr/local/Cellar/cryptopp/5.6.3_1/include/cryptopp/ -I src/include/
CLIBS   = -lcryptopp

# the build target executable:
TARGET = main

all: $(TARGET)

$(TARGET): src/$(TARGET).cpp src/secret_sharing.cpp
	$(CC) $(CFLAGS) -o bin/$(TARGET) src/$(TARGET).cpp src/secret_sharing.cpp src/opts.cpp $(CLIBDIR) $(CHEADER_DIR) $(CLIBS)

#bin/main: src/$(TARGET).cpp 	# this is not working, how to make bin/main depend on src/$(TARGET)

run: bin/$(TARGET)
	./bin/main $(ARGS)

clean:
	$(RM) $(TARGET)
