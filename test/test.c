#include <stdio.h>
#include <stdlib.h>

#include "../src/octree.h"

int
main()
{
    float* octree_position = malloc(3 * sizeof *octree_position);
    octree_position[0] = 100;
    octree_position[1] = 101;
    octree_position[2] = 102;
    OctreeContainer* octree = oct_octree_init(octree_position, 100);
    printf("%f\n", octree->position[1]);

    return 0;
}
