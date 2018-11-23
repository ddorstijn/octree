CC = gcc
OS := $(shell uname)
APP_NAME := octree

src_test = src/test/main.c
obj_test = $(src_test:.c=.o)

src = $(wildcard src/*.c)
obj = $(src:.c=.o)

LDFLAGS =
CFLAGS = -shared

ifeq ("$(DEBUG)","1")
CFLAGS += -g -O0
BUILD_DIR := bin/debug/
else
CFLAGS += -O3
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

test: $(obj) $(obj_test)
	$(CC) -g -o0 $^ -o src/test/test 

clean:
	rm -f $(obj) win32

.PHONY: default test clean
