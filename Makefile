CC ?= cc
CFLAGS ?= -std=gnu11 -Wall -Wextra -Wpedantic -g -O0

SOURCES = src/server.c src/resp.c src/db.c src/commands.c
HEADERS = src/resp.h src/db.h src/commands.h
TARGET = redis_clone

.PHONY: all clean asan

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc -o $@ $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)

asan: clean
	$(MAKE) CFLAGS="$(CFLAGS) -fsanitize=address,undefined -g -O0" $(TARGET)
