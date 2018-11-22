CC = gcc
OS := $(shell uname)

src = $(wildcard src/*.c)
obj = $(src:.c=.o)

LDFLAGS =
CFLAGS = -shared

ifeq ($(OS), Linux)
CFLAGS += -fPIC

default: linux
else
default: win32
endif

win32: $(obj)
	$(CC) $(CFLAGS) -DEXPORT_SHARED -o bin/octree.dll $^ $(LDFLAGS)
	
linux: $(obj)
	$(CC) $(CFLAGS) -o bin/liboctree.so $^ $(LDFLAGS)

clean:
	rm -f $(obj) win32

.PHONY: default win32 clean
