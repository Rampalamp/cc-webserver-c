CC = gcc

CFLAGS = -Wall -Wextra -std=c17 -Iinclude

CFLAGS += -D_POSIX_C_SOURCE=200809L
CFLAGS += -D_DEFAULT_SOURCE

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