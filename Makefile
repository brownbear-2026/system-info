CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall
TARGET = system_info
SRC = system_Info.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
