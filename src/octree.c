#include "octree.h"

#include <assert.h>
#include <limits.h>

void
oct_octree_build(OctreeContainer* octree, float** object_positions,
                 size_t object_count)
{
    for (size_t i = 0; i < object_count; i++) {

        uint8_t child = 0;
        child |= object_positions[i][0] < octree->position[0] ? 0 : 0b001;
        child |= object_positions[i][1] < octree->position[1] ? 0 : 0b010;
        child |= object_positions[i][2] < octree->position[2] ? 0 : 0b100;

        if ((octree->root_node->type != NodeType.LEAF) &&
            ((octree->root_node->data.child_exists & child) == child)) {
            oct_node_split(child, object);
        }
    }
}

size_t
hash_func(void* key)
{
    return (size_t)key;
}

bool
equals_func(void* key1, void* key2)
{
    return (size_t)key1 == (size_t)key1;
}

OctreeContainer*
oct_octree_init(float* position)
{
    OctreeContainer* octree = malloc(sizeof *octree);
    octree->position = position;
    octree->total_node_count = 0;
    octree->nodes = unordered_map_alloc(10000000, 0, hash_func, equals_func);
    octree->root_node = oct_node_init(0, ULLONG_MAX, NodeType.LEAF);

    return octree;
}

OctreeNode*
oct_node_init(uint64_t location_code, uint64_t object_index, NodeType type)
{
    OctreeNode* node = malloc(sizeof *node);
    node->location_code = location_code;
    if (type == NodeType.INNER) {
        node->data.child_exists = 0b00;
    } else {
        node->data.object_index = object_index;
    }
    node->type = (uint8_t)type;

    return node;
}

size_t
oct_node_get_position(OctreeNode* node)
{
    return 0;
}

size_t
oct_node_get_tree_depth(OctreeContainer* octree, const OctreeNode* node)
{
    assert(node->location_code); // at least flag bit must be set

#if defined(__GNUC__)
    return (63 - __builtin_clz(node->location_code)) / 3;
#elif defined(_MSC_VER)
    long msb;
    _BitScanReverse(&msb, node->location_code);
    return msb / 3;
#else
    size_t depth;
    for (uint32_t lc = node->location_code, depth = 0; lc != 1;
         lc >>= 3, depth++)
        ;
    return depth;
#endif
}

OctreeNode*
oct_node_get_parent(OctreeContainer* octree, OctreeNode* node)
{
    uint64_t location_code_parent = node->location_code >> 3;
    return oct_node_lookup(octree, location_code_parent);
}

OctreeNode*
oct_node_lookup(OctreeContainer* octree, uint64_t location_code)
{
    OctreeNode* node =
        (OctreeNode*)unordered_map_get(octree->nodes, &location_code);
    return node;
}

// TEST CASE
void
oct_visit_all(OctreeContainer* octree, OctreeNode* node)
{
    for (int i = 0; i < 8; i++) {
        if (node->data.child_exists & (1 << i)) {
            uint32_t locCodeChild = (node->location_code << 3) | i;
            OctreeNode* child = oct_node_lookup(octree, locCodeChild);
            oct_visit_all(octree, child);
        }
    }
}
