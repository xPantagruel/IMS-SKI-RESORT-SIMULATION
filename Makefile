CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LIBS = -lm -lsimlib

SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = simulation

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(EXEC)