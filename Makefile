CXX = g++
CXXFLAGS = -std=c++17 -I src -I utils

SRC = \
	src/Solucao/main.cpp \
	src/Solucao/iterated_local_search.cpp \
	src/Solucao/solucao.cpp \
	src/Data/Data.cpp \
	src/Subsequencia/subsequencia.cpp

OBJ = $(SRC:.cpp=.o)
TARGET = a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
