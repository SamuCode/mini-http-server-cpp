CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

# SRC = src/main.cpp src/server.cpp
SRC = $(wildcard src/*.cpp)
OUT = server

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)