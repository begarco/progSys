# definition des cibles particulieres
.PHONY: clean

# definition des variables
CXX = gcc
CXXFLAGS = -O2 -Wall -Wextra -g -std=c99 -lpthread
CXXFLAGS2 = -O2 -Wall -pedantic -ansi -Wextra -pg -std=c99

all : suisse horizontal vertical original parallele clean

suisse : suisse.c
	$(CXX) $^ -o ./$@ $(CXXFLAGS)

horizontal : horizontal.c
	$(CXX) $^ -o ./$@ $(CXXFLAGS)

vertical : vertical.c
	$(CXX) $^ -o ./$@ $(CXXFLAGS)

parallele : parallele.c
	$(CXX) $^ -o ./$@ $(CXXFLAGS)

original : blur.c
	$(CXX) $^ -o ./$@ $(CXXFLAGS)

clean :
	rm -rf *.bak; rm -rf *.o
