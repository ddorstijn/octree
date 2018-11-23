CC = gcc
OS := $(shell uname)
APP_NAME := test

src = main.c
obj = $(src:.c=.o)

LDFLAGS = -L../bin/debug -loctree -Wl,-rpath '-Wl,$$ORIGIN'
CFLAGS = -g

default: $(obj)
	$(CC) $(CFLAGS) -o $(APP_NAME) $^ $(LDFLAGS)

clean:
	rm -f $(obj) default

.PHONY: default clean
