CC = gcc
OS := $(shell uname)

src = main.c
obj = $(src:.c=.o)

LDFLAGS = -L../bin -loctree -Wl,-rpath '-Wl,$$ORIGIN'
CFLAGS = 

ifeq ($(OS), Linux)
default: linux
else
default: win32
endif

win32: $(obj)
	$(CC) $(CFLAGS) -o main $^ $(LDFLAGS)

linux: $(obj)
	$(CC) $(CFLAGS) -o main $^ $(LDFLAGS)

.PHONY: default win32 linux clean
