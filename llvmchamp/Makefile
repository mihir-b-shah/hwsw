
TARGET=tracer
CXX=g++
CLD=g++
SRC_FILES=$(wildcard *.cc)
OBJ_FILES=$(patsubst %.cc,%.o,$(SRC_FILES))
HEADER_FILES=$(wildcard *.h)
CXX_FLAGS=$(shell llvm-config --cxxflags)
CLD_FLAGS=$(shell llvm-config --ldflags --libs)

%.o: %.cc $(HEADER_FILES)
	$(CXX) -c -O3 $(CXX_FLAGS) $< -o $@

$(TARGET): $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) $(CLD_FLAGS) -o $(TARGET)

all: $(TARGET)

clean:
	rm *.o $(TARGET)
