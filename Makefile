CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2


RAYLIB_FLAGS = $(shell pkg-config --cflags --libs raylib) -lopengl32 -lgdi32 -lwinmm 

TARGET = main.exe
SRC = main.c

.PHONY: all clean run

all: clean $(TARGET) run

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(RAYLIB_FLAGS)

run:
	.\$(TARGET)

clean:
	del *.exe