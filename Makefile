# Makefile for Hardware Stress Test Suite
# MIT License (c) 2026 kj-devvixon

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -march=native -pthread
INCLUDES = -I./include
SOURCES = src/main.cpp src/stress_test.cpp
TARGET = stress_test

.PHONY: all clean install run help

all: $(TARGET)

$(TARGET): $(SOURCES)
	@echo "Building Hardware Stress Test Suite..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET)
	@echo "Build complete! Run with: ./$(TARGET)"

debug: CXXFLAGS += -g -DDEBUG -O0
debug: clean $(TARGET)
	@echo "Debug build complete!"

release: CXXFLAGS += -DNDEBUG
release: clean $(TARGET)
	@echo "Release build complete!"

clean:
	@echo "Cleaning build artifacts..."
	rm -f $(TARGET)
	rm -rf build/
	@echo "Clean complete!"

install: $(TARGET)
	@echo "Installing to /usr/local/bin..."
	sudo cp $(TARGET) /usr/local/bin/
	@echo "Installation complete! Run with: stress_test"

uninstall:
	@echo "Uninstalling..."
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "Uninstall complete!"

run: $(TARGET)
	@echo "Starting Hardware Stress Test Suite..."
	./$(TARGET)

help:
	@echo "Hardware Stress Test Suite - Build System"
	@echo "=========================================="
	@echo ""
	@echo "Available targets:"
	@echo "  make          - Build the application (default)"
	@echo "  make debug    - Build with debug symbols"
	@echo "  make release  - Build optimized release"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make install  - Install to system (requires sudo)"
	@echo "  make uninstall- Remove from system (requires sudo)"
	@echo "  make run      - Build and run the application"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Usage:"
	@echo "  ./stress_test [port]"
	@echo ""
	@echo "Example:"
	@echo "  make && ./stress_test 8080"
	@echo ""
