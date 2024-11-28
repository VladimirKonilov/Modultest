# Compiler and Flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I/usr/include/cryptopp -I/usr/include/UnitTest++ -L/usr/lib/x86_64-linux-gnu

# Libraries
LIBS = -lcryptopp -lUnitTest++

# Source Files
SRCS = client_tests.cpp Communicator.cpp

# Output File
TARGET = client_tests

# Default target to build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

# Clean up generated files
clean:
	rm -f $(TARGET)

# Run tests
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
