CC = gcc

CFLAGS = -Wall -Wextra -std=c17 -Iinclude

SRC = $(wildcard src/*.c)
TEST = $(wildcard test/*.c)

TARGET = build/program

all: $(TARGET)

$(TARGET): $(SRC) $(TEST)
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) $(TEST) -o $(TARGET)

clean:
	rm -rf build

run: all
	./$(TARGET)

.PHONY: all clean run