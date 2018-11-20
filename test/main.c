#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "octree.h"

int
main()
{
    size_t idx = 5;
    assert(hash_func(&idx) == 5);
    size_t idx2 = 5;
    assert(equals_func(&idx, &idx2) == true);
    
    float* octree_position = malloc(3 * sizeof *octree_position);
    octree_position[0] = 100;
    octree_position[1] = 101;
    octree_position[2] = 102;
    OctreeContainer* octree = oct_octree_init(octree_position, 100);
    assert(octree->position[0] == octree_position[0]);

    int r = 5, c = 3, i, j, count;

    float* positions[r];
    for (i = 0; i < r; i++)
        positions[i] = malloc(c * sizeof(float));

    count = 0;
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            positions[i][j] = ++count;
        }
    }
    
    oct_octree_build(octree, positions, r);

    unordered_map_iterator* iterator = unordered_map_iterator_alloc(octree->nodes);
    while (unordered_map_iterator_has_next(iterator) > 0) {
        void* key_pointer; 
        void* value_pointer;
        unordered_map_iterator_next(iterator, &key_pointer, &value_pointer);
        printf("%I64u\n", *(size_t*)key_pointer);
        printf("%u\n", ((OctreeBaseNode*)value_pointer)->type);
    }

    /* oct_octree_build(octree, positions, r); */

    /* oct_node_init_inner(); */

    /* oct_node_init_leaf(); */

    /* oct_node_split_leaf_node(); */

    /* oct_find_leaf_node(); */

    oct_octree_free(octree);
    return 0;
}
