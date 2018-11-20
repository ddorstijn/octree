CC = gcc

src = $(wildcard src/*.c)
obj = $(src:.c=.o)

LDFLAGS =
CFLAGS = -shared

win32: $(obj)
	$(CC) $(CFLAGS) -o bin/liboctree.dll $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) win32