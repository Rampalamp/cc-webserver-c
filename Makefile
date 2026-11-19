CC = gcc

CPPFLAGS = -Iinclude -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE -MMD -MP
CFLAGS = -std=c17 -Wall -Wextra

# Usage: make (debug, the default) or make BUILD=release
BUILD ?= debug

ifeq ($(BUILD),debug)
    CFLAGS += -g -O0 -fsanitize=address,undefined
    LDFLAGS += -fsanitize=address,undefined
else ifeq ($(BUILD),release)
    CFLAGS += -O2
    CPPFLAGS += -DNDEBUG
else
    $(error BUILD must be 'debug' or 'release')
endif

CPPFLAGS += -I$(HOME)/.local/include
LDFLAGS += -L$(HOME)/.local/lib
LDLIBS = -lccutils

BUILD_DIRECTORY = build/$(BUILD)

SOURCE_FILES = $(wildcard src/*.c)
OBJECT_FILES = $(SOURCE_FILES:src/%.c=$(BUILD_DIRECTORY)/%.o)

TEST_SOURCE_FILES = $(wildcard test/*.c)
TEST_OBJECT_FILES = $(TEST_SOURCE_FILES:test/%.c=$(BUILD_DIRECTORY)/test_%.o)

# Everything except main.c, so tests can link against the server code
LIBRARY_OBJECT_FILES = $(filter-out $(BUILD_DIRECTORY)/main.o,$(OBJECT_FILES))

DEPENDENCY_FILES = $(OBJECT_FILES:.o=.d) $(TEST_OBJECT_FILES:.o=.d)

TARGET = $(BUILD_DIRECTORY)/ccwebserver
TEST_TARGET = $(BUILD_DIRECTORY)/run_tests
ARGS ?=

.PHONY: all release test run clean

all: $(TARGET)

release:
	$(MAKE) BUILD=release all

$(TARGET): $(OBJECT_FILES)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(TEST_TARGET): $(TEST_OBJECT_FILES) $(LIBRARY_OBJECT_FILES)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(BUILD_DIRECTORY)/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIRECTORY)/test_%.o: test/%.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

run: all
	./$(TARGET) $(ARGS)

clean:
	rm -rf build

-include $(DEPENDENCY_FILES)