
#g++ -g -Wall -o bin/main src/main.cpp

# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall
CLIBDIR = -L /usr/local/Cellar/cryptopp/5.6.3_1/lib/ 
CHEADER_DIR = -I /usr/local/Cellar/cryptopp/5.6.3_1/include/cryptopp/
CLIBS   = -lcryptopp

# the build target executable:
TARGET = main

all: $(TARGET)

$(TARGET): src/$(TARGET).cpp
	$(CC) $(CFLAGS) -o bin/$(TARGET) src/$(TARGET).cpp $(CLIBDIR) $(CHEADER_DIR) $(CLIBS)

clean:
	$(RM) $(TARGET)
