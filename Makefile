# Makefile for HTML Renderer with DSA
# SDL2 is optional - the program runs in console-only mode without it

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

# Detect OS
UNAME_S := $(shell uname -s)

# Check if SDL2 should be enabled (set USE_SDL2=1 to enable)
USE_SDL2 ?= 0

ifeq ($(USE_SDL2),1)
    LDFLAGS = -lSDL2 -lSDL2_ttf
    CXXFLAGS += -DUSE_SDL2
    
    # macOS specific paths
    ifeq ($(UNAME_S),Darwin)
        CXXFLAGS += -I/opt/homebrew/include -I/usr/local/include
        LDFLAGS += -L/opt/homebrew/lib -L/usr/local/lib
    endif
    
    # Linux specific paths
    ifeq ($(UNAME_S),Linux)
        CXXFLAGS += -I/usr/include/SDL2
        LDFLAGS += -L/usr/lib/x86_64-linux-gnu
    endif
else
    LDFLAGS =
    $(info Building without SDL2 - console-only mode)
    $(info To enable SDL2: make USE_SDL2=1)
endif

TARGET = html_renderer
SOURCES = html_renderer.cpp
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET) parsed_output.txt

install-deps-macos:
	@echo "Installing SDL2 dependencies on macOS..."
	@echo "Run: brew install sdl2 sdl2_ttf"

install-deps-linux:
	@echo "Installing SDL2 dependencies on Linux..."
	@echo "Run: sudo apt-get install libsdl2-dev libsdl2-ttf-dev"

help:
	@echo "Makefile for HTML Renderer"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build the renderer without SDL2 (console-only, default)"
	@echo "  clean        - Remove build artifacts"
	@echo "  run          - Build and run with parsed_output.txt"
	@echo "  install-deps-macos  - Show instructions for macOS dependencies"
	@echo "  install-deps-linux  - Show instructions for Linux dependencies"
	@echo ""
	@echo "Usage:"
	@echo "  make                    - Build without SDL2 (console-only mode)"
	@echo "  make USE_SDL2=1         - Build with SDL2 support"
	@echo "  make run                - Build and run"
	@echo "  ./html_renderer <file>  - Run with custom file"
	@echo ""
	@echo "Note: SDL2 is optional. The program shows all DSA demonstrations"
	@echo "      via console output even without SDL2."

