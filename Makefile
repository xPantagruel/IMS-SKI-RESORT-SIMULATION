CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LIBS = -lm -lsimlib

SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = sim

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: $(EXEC)
	./$(EXEC)

clean:
	rm -f $(OBJS) $(EXEC)