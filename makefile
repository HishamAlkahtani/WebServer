CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude -Isrc
LDFLAGS = -lspdlog -lfmt -pthread

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
SOURCES += $(wildcard $(SRC_DIR)/http/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
TARGET = $(BIN_DIR)/WebServer

all: directories $(TARGET)

directories:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(BUILD_DIR)/http

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)