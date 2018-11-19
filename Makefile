SOURCES = src/octree.c src/unordered_map.c

default:
	gcc -shared -fPIC -o liboctree.so $(SOURCES)

testno:
	gcc -g -std=c11 src/main.c $(SOURCES) -o test_nodll 

test:
	gcc test/test.c -o oct_test.out -Lbin/ -loctree

.PHONY: default test testno
