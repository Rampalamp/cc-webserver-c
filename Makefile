CC = gcc

CFLAGS = -Wall -Wextra -std=c17 -Iinclude

CFLAGS += -D_POSIX_C_SOURCE=200809L
CFLAGS += -D_DEFAULT_SOURCE

SRC = $(wildcard src/*.c)
TEST = $(wildcard test/*.c)

TARGET = build/ccwebserver
ARGS ?=

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC) $(TEST)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf build

run: all
	./$(TARGET) $(ARGS)
