CXX := g++
CXXFLAGS := -Wall -O3 -std=c++17
INCLUDE := -I./

SRCS := $(wildcard ./*.cpp)
OBJS := $(SRCS:.cpp=.o)
LIBS := -lpthread
TARGET := tetris

$(TARGET): $(OBJS)
	$(CXX) $(INCLUDE) -o $@ $(OBJS) $(CXXFLAGS) $(LIBS)

%.o: %.c
	$(CXX) $(INCLUDE) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)

