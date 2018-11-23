CC = gcc
OS := $(shell uname)
APP_NAME := test

src = src/main.c
obj = $(src:.c=.o)

BUILD_DIR := bin/

CFLAGS = -g
LDFLAGS = -L../bin/debug -loctree -Wl,-rpath '-Wl,$$ORIGIN'

default: $(obj)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)$(APP_NAME) $^ $(LDFLAGS)

clean:
	rm -f $(obj) default

.PHONY: default clean
