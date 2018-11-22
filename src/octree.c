#include "octree.h"

#include <limits.h>

size_t
hash_func(void* key)
{
    return *(size_t*)key;
}

bool
equals_func(void* key1, void* key2)
{
    return (*(size_t*)key1) == (*(size_t*)key2);
}

OctreeContainer*
oct_octree_init(Position position, size_t size)
{
    OctreeContainer* octree = malloc(sizeof *octree);
    if (octree == NULL) {
        return NULL;
    }

    octree->position = position;
    octree->size = size;
    octree->leaf_count = 0;
    octree->inner_count = 0;
    octree->nodes = unordered_map_alloc(10000000, 0, hash_func, equals_func);
    octree->root_node = oct_node_init_leaf(octree, 0, 1, ULLONG_MAX);
    if (octree->root_node == NULL) {
        return NULL;
    }

    return octree;
}

void
oct_octree_free(OctreeContainer* octree)
{
    unordered_map_clear(octree->nodes);
    free(octree);
}

void
oct_octree_build(OctreeContainer* octree, Position* object_positions,
                 size_t object_count)
{
    octree->object_positions = object_positions;

    for (size_t i = 0; i < object_count; i++) {
        OctreeLeafNode* leaf_node =
            oct_find_leaf_node(octree, octree->root_node, object_positions[i]);
        if (leaf_node->object_index != ULLONG_MAX) {
            oct_node_split_leaf_node(octree, leaf_node);
        } else {
            leaf_node->object_index = i;
        }
    }
}

OctreeInnerNode*
oct_node_init_inner(OctreeContainer* octree, uint64_t location_code)
{
    OctreeInnerNode* node = malloc(sizeof *node);
    if (node == NULL) {
        /* printf("Error creating node, malloc failed"); */
        return NULL;
    }

    node->base.location_code = location_code;
    node->base.type = INNER_NODE;
    node->child_exists = 0b00;

    unordered_map_put(octree->nodes, &node->base.location_code, node);
    octree->inner_count++;

    // If we just removed the root node set it to the new inner node
    if (location_code == 0b1) {
        octree->root_node = node;
    }

    return node;
}

void
oct_free_inner(OctreeContainer* octree, uint64_t location_code) 
{
    unordered_map_remove(octree->nodes, &location_code);
    octree->inner_count--;
}

OctreeLeafNode*
oct_node_init_leaf(OctreeContainer* octree, uint64_t parent_location,
                   uint8_t child_location, uint64_t object_index)
{
    OctreeLeafNode* node = malloc(sizeof *node);
    if (node == NULL) {
        /* printf("Error creating node, malloc failed"); */
        return NULL;
    }

    uint64_t new_location = (parent_location << 3) | child_location;
    node->base.location_code = new_location;
    node->base.type = LEAF_NODE;
    node->object_index = object_index;

    unordered_map_put(octree->nodes, &node->base.location_code, node);
    octree->leaf_count++;

    if (parent_location) {
        OctreeInnerNode* parent_node =
            (OctreeInnerNode*)oct_node_lookup(octree, parent_location);
        parent_node->child_exists |= (1u << child_location);
    }

    // If we just removed the root node set it to the new inner node
    if (new_location == 0b1) {
        octree->root_node = node;
    }

    return node;
}

void
oct_free_leaf(OctreeContainer* octree, uint64_t location_code) 
{
    unordered_map_remove(octree->nodes, &location_code);
    octree->leaf_count--;
}

OctreeLeafNode*
oct_find_leaf_node(OctreeContainer* octree, OctreeBaseNode* node,
                   Position object_position)
{
    switch (node->type) {
        case LEAF_NODE:
            return (OctreeLeafNode*)node;
            break;
        case INNER_NODE: {
            Position node_position = oct_node_get_position(octree, node);

            uint8_t child_location = 0;
            child_location |= object_position.x < node_position.x ? 0 : 0b001;
            child_location |= object_position.y < node_position.y ? 0 : 0b010;
            child_location |= object_position.z < node_position.z ? 0 : 0b100;

            if (((OctreeInnerNode*)node)->child_exists &
                (1u << child_location)) {
                return oct_find_leaf_node(
                    octree,
                    oct_node_get_child(octree, node->location_code,
                                       child_location),
                    object_position);
            }

            return oct_node_init_leaf(octree, node->location_code,
                                      child_location, ULLONG_MAX);

            break;
        }
        default: 
            // If there was an error with the node type exit switch and return NULL
            break;
    }

    return NULL;
}

OctreeLeafNode*
oct_node_split_leaf_node(OctreeContainer* octree, OctreeLeafNode* node)
{
    uint64_t object_index = node->object_index;
    uint64_t location_code = node->base.location_code;
    oct_free_leaf(octree, location_code);

    OctreeInnerNode* inner_node = oct_node_init_inner(octree, location_code);
    if (node == NULL) {
        return NULL;
    }

    OctreeLeafNode* new_child =
        oct_find_leaf_node(octree, (OctreeBaseNode*)inner_node,
                           octree->object_positions[object_index]);
    new_child->object_index = object_index;

    return new_child;
}

Position
oct_node_get_position(OctreeContainer* octree, OctreeBaseNode* node)
{
    Position position = octree->position;

    int tree_depth = oct_node_get_tree_depth(octree, node);
    for (int i = 0; i < tree_depth; i += 3) {
        uint64_t offset = 1;
        offset = offset << (tree_depth * 3 - 3 * i);
        uint8_t local_code = node->location_code & offset;
        position.x = (local_code & 0b001)
                          ? position.x + octree->size / (2.0f * tree_depth)
                          : position.x - octree->size / (2.0f * tree_depth);
        position.y = (local_code & 0b010)
                          ? position.y + octree->size / (2.0f * tree_depth)
                          : position.y - octree->size / (2.0f * tree_depth);
        position.z = (local_code & 0b100)
                          ? position.z + octree->size / (2.0f * tree_depth)
                          : position.z - octree->size / (2.0f * tree_depth);
    }

    return position;
}

size_t
oct_node_get_tree_depth(OctreeContainer* octree, const OctreeBaseNode* node)
{
#if defined(__GNUC__)
    return (63 - __builtin_clzll(node->location_code)) / 3;
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

OctreeBaseNode*
oct_node_get_parent(OctreeContainer* octree, OctreeBaseNode* node)
{
    uint64_t location_code_parent = node->location_code >> 3;
    return oct_node_lookup(octree, location_code_parent);
}

OctreeBaseNode*
oct_node_get_child(OctreeContainer* octree, uint64_t location_code,
                   uint8_t child_location)
{
    uint64_t child_location_code = (location_code << 3) | child_location;
    return oct_node_lookup(octree, child_location_code);
}

OctreeBaseNode*
oct_node_lookup(OctreeContainer* octree, uint64_t location_code)
{
    return unordered_map_get(octree->nodes, &location_code);
}

size_t 
oct_octree_get_size(OctreeContainer* octree)
{
    return octree->size;
}

size_t 
oct_octree_get_leaf_count(OctreeContainer* octree)
{
    return octree->leaf_count;
}

size_t 
oct_octree_get_inner_count(OctreeContainer* octree)
{
    return octree->inner_count;
}

// TEST CASE
void
oct_visit_all(OctreeContainer* octree, OctreeBaseNode* node)
{
    for (int i = 0; i < 8; i++) {
        if (node->type == INNER_NODE) {
            if (((OctreeInnerNode*)node)->child_exists & (1 << i)) {
                OctreeBaseNode* child =
                    oct_node_get_child(octree, node->location_code, i);
                oct_visit_all(octree, child);
            }
        } else {
            /* printf("%p\n", node); */
        }
    }
}
