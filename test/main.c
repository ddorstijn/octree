#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/octree.h"

#define ROWS 5

int
main()
{
    size_t idx = 5;
    assert(hash_func(&idx) == 5);
    size_t idx2 = 5;
    assert(equals_func(&idx, &idx2) == true);

    int octree_size = 100;
    Position octree_position = {100, 101, 102};
    Octree* octree = oct_octree_init(octree_position, octree_size);
    
    int count = 0;
    Position positions[ROWS];
    for (int i = 0; i < ROWS; i++) {
        positions[i].x = ++count;
        positions[i].y = ++count;
        positions[i].z = ++count;
    }

    oct_octree_build(octree, positions, ROWS);

    unordered_map_iterator* iterator =
        unordered_map_iterator_alloc(octree->nodes);
    
    while (unordered_map_iterator_has_next(iterator) > 0) {
        void* key_pointer;
        void* value_pointer;
        unordered_map_iterator_next(iterator, &key_pointer, &value_pointer);
        printf("%u\n", ((BaseNode*)value_pointer)->type);
    }

    oct_octree_free(octree);

    return 0;
}
