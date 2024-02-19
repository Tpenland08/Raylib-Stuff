CC = cc
CFLAGS = -I/opt/homebrew/Cellar/raylib/5.0/include
LDFLAGS = -L/opt/homebrew/Cellar/raylib/5.0/lib
LIBS = -lraylib

SRC = main.c
TARGET = main

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
