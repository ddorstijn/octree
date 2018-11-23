CC = gcc
OS := $(shell uname)
APP_NAME := octree

src = $(wildcard src/*.c)
obj = $(src:.c=.o)

LDFLAGS =
CFLAGS = -shared

ifeq ("$(DEBUG)","1")
CFLAGS += -g
BUILD_DIR := bin/debug/
else
BUILD_DIR := bin/release/
endif

ifeq ($(OS), Linux)
CFLAGS += -fPIC
SUFFIX := .so
else
CFLAGS += -DEXPORT_SHARED
SUFFIX := .dll
endif

default: $(obj)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)lib$(APP_NAME)$(SUFFIX) $^ $(LDFLAGS)

clean:
	rm -f $(obj) win32

.PHONY: default clean
