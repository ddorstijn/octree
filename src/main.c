#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "octree.h"

int
main()
{
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

    oct_visit_all(octree, octree->root_node);

    auto iterator = unordered_map_iterator_alloc(octree->nodes);
    while (unordered_map_iterator_next(iterator)) {
        printf(iterator->next_entry);
    }
    /* oct_octree_build(octree, positions, r); */

    /* oct_node_init_inner(); */

    /* oct_node_init_leaf(); */

    /* oct_node_split_leaf_node(); */

    /* oct_find_leaf_node(); */

    oct_octree_free(octree);
    return 0;
}
