CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -static -I./include
SRCS = lexer.cpp parser.cpp ir_visitor.cpp main.cpp \
       src/BasicBlock.cpp src/Constant.cpp src/Function.cpp \
       src/GlobalVariable.cpp src/Instruction.cpp src/IRprinter.cpp \
       src/Module.cpp src/Type.cpp src/User.cpp src/Value.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = mycompiler.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo 正在清理编译产物...
	@if exist *.o del /Q *.o
	@if exist src\*.o del /Q src\*.o
	@if exist mycompiler.exe del /Q mycompiler.exe