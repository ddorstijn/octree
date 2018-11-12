#include "octree.h"

#include <assert.h>

void
oct_octree_build(OctreeContainer* octree, float** object_positions, size_t object_count)
{
    for (size_t i = 0; i < object_count; i++) {
        
    }
} 

size_t 
hash_function(void* key) 
{
    return (size_t)key;
}

bool 
equals_function(void* key1, void* key2)
{
    return (size_t)key1 == (size_t)key1;
}

OctreeContainer*
oct_octree_init(float* center_position) 
{
    OctreeContainer* octree = malloc(sizeof *octree);
    octree->center_position = center_position;
    octree->total_node_count = 0;
    octree->nodes = unordered_map_alloc(10000000, 0, hash_function, equals_function);

    return octree;
}

OctreeNode* 
oct_node_init(uint64_t object_index, uint64_t location_code) 
{
    OctreeNode* node = malloc(sizeof *node);
    node->location_code = location_code;
    node->object_index = object_index;
    node->child_exists = 0b00;

    return node;
}

size_t oct_node_get_position(OctreeNode* node)
{
    return 0;
}

size_t 
oct_node_get_tree_depth(OctreeContainer* octree, const OctreeNode *node)
{
    assert(node->location_code); // at least flag bit must be set
    
#if defined(__GNUC__)
    return (63-__builtin_clz(node->location_code))/3;
#elif defined(_MSC_VER)
    long msb;
    _BitScanReverse(&msb, node->location_code);
    return msb/3;
#else
    size_t depth;
    for (uint32_t lc = node->location_code, depth = 0; lc != 1; lc >>= 3, depth++);
        return depth; 
#endif
}

OctreeNode* 
oct_node_get_parent(OctreeContainer* octree, OctreeNode* node)
{
    uint64_t location_code_parent = node->location_code>>3;
    return oct_node_lookup(octree, location_code_parent);
}
 
OctreeNode*
oct_node_lookup(OctreeContainer* octree, uint64_t location_code)
{
    OctreeNode* node = (OctreeNode*)unordered_map_get(octree->nodes, &location_code);
    return node;
}


// TEST CASE
void 
oct_visit_all(OctreeContainer* octree, OctreeNode *node)
{
    for (int i=0; i<8; i++)
    {
        if (node->child_exists & (1<<i))
        {
            uint32_t locCodeChild = (node->location_code << 3) | i;
            OctreeNode* child = oct_node_lookup(octree, locCodeChild);
            oct_visit_all(octree, child);
        }
    }
}