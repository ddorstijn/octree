SOURCES = src/octree.c src/unordered_map.c

default:
	gcc -Wall -shared $(SOURCES) -o octree.dll
