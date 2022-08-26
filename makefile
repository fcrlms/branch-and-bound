CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror -pedantic

SOURCES = $(wildcard *.cpp)

OBJECTS = $(SOURCES: %.cpp = %.o)

EXECUTABLE = bnb.out

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXECUTABLE)

%.o:
	$(CXX) $(CXXFLAGS) -c $*.cpp
