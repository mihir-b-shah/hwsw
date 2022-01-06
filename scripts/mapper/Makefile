
TARGET=mfnum
CXX=g++
CLD=g++
LLVM_BIN_PATH=/usr/lib/llvm-13/bin
SRC_FILES=$(wildcard *.cc)
OBJ_FILES=$(patsubst %.cc,%.o,$(SRC_FILES))
HEADER_FILES=$(wildcard *.h)
CXX_FLAGS=$(shell $(LLVM_BIN_PATH)/llvm-config --cxxflags)
CLD_FLAGS=$(shell $(LLVM_BIN_PATH)/llvm-config --ldflags --libs)

%.o: %.cc $(HEADER_FILES)
	$(CXX) -c -g -O0 $(CXX_FLAGS) $< -o $@

$(TARGET): $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) $(CLD_FLAGS) -g -o $(TARGET)

all: $(TARGET)

clean:
	rm *.o $(TARGET)
