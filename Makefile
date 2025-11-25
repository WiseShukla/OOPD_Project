# Makefile for Cellular Network Simulator
CXX = g++
ASM = nasm

# Source files
SOURCES = Simulator.cpp UserDevice.cpp CellularCore.cpp \
          CellTower.cpp StringUtils.cpp basicIO.cpp ConfigParser.cpp
ASM_FILE = syscall.S

# Output binaries
DEBUG_BIN = simulator_debug
RELEASE_BIN = simulator

# Compiler flags
DEBUG_FLAGS = -std=c++17 -g -O0 -Wall -pthread
RELEASE_FLAGS = -std=c++17 -O3 -Wall -pthread

# Default target
all: debug release

# Debug version
debug: $(SOURCES) $(ASM_FILE)
	$(ASM) -f elf64 $(ASM_FILE) -o syscall_debug.o
	$(CXX) $(DEBUG_FLAGS) $(SOURCES) syscall_debug.o -o $(DEBUG_BIN) -no-pie

# Release version
release: $(SOURCES) $(ASM_FILE)
	$(ASM) -f elf64 $(ASM_FILE) -o syscall_release.o
	$(CXX) $(RELEASE_FLAGS) $(SOURCES) syscall_release.o -o $(RELEASE_BIN) -no-pie

# Run debug version
run-debug: debug
	./$(DEBUG_BIN) input.txt

# Run release version
run: release
	./$(RELEASE_BIN) input.txt

# Clean all build files
clean:
	rm -f $(DEBUG_BIN) $(RELEASE_BIN) *.o

.PHONY: all debug release run-debug run clean