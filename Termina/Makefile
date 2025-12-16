CXX = g++
RAYLIB_FLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
SOURCES = terminal.cpp terminalControl.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = r

all:$(TARGET)

%o:%cpp
	$(CXX) -cpp %< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -lcjson -o $(TARGET) $(RAYLIB_FLAGS)

clean:
	rm -r $(TARGET) $(OBJECTS)
