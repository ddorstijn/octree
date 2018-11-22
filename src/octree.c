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

Octree*
oct_octree_init(Position position, size_t size)
{
    Octree* octree = malloc(sizeof *octree);
    if (octree == NULL) {
        return NULL;
    }

    octree->position = position;
    octree->size = size;
    octree->leaf_count = 0;
    octree->inner_count = 0;
    octree->nodes = unordered_map_alloc(10000000, 0, hash_func, equals_func);
    octree->root_node = oct_leaf_node_init(octree, 0, 1, ULLONG_MAX);
    if (octree->root_node == NULL) {
        return NULL;
    }

    return octree;
}

void
oct_octree_free(Octree* octree)
{
    unordered_map_clear(octree->nodes);
    free(octree);
}

void
oct_octree_build(Octree* octree, Position* object_positions,
                 size_t object_count)
{
    octree->object_positions = object_positions;

    for (size_t i = 0; i < object_count; i++) {
        LeafNode* leaf_node =
            oct_leaf_node_find(octree, octree->root_node, object_positions[i]);
        if (leaf_node->object_index != ULLONG_MAX) {
            oct_leaf_node_split(octree, leaf_node);
        } else {
            leaf_node->object_index = i;
        }
    }
}

BranchNode*
oct_branch_node_init(Octree* octree, uint64_t location_code)
{
    BranchNode* node = malloc(sizeof *node);
    if (node == NULL) {
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
oct_branch_node_free(Octree* octree, uint64_t location_code) 
{
    unordered_map_remove(octree->nodes, &location_code);
    octree->inner_count--;
}

LeafNode*
oct_leaf_node_init(Octree* octree, uint64_t parent_location,
                   uint8_t child_location, uint64_t object_index)
{
    LeafNode* node = malloc(sizeof *node);
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
        BranchNode* parent_node =
            (BranchNode*)oct_node_lookup(octree, parent_location);
        parent_node->child_exists |= (1u << child_location);
    }

    // If we just removed the root node set it to the new inner node
    if (new_location == 0b1) {
        octree->root_node = node;
    }

    return node;
}

void
oct_leaf_node_free(Octree* octree, uint64_t location_code) 
{
    unordered_map_remove(octree->nodes, &location_code);
    octree->leaf_count--;
}

LeafNode*
oct_leaf_node_find(Octree* octree, BaseNode* node,
                   Position object_position)
{
    switch (node->type) {
        case LEAF_NODE:
            return (LeafNode*)node;
            break;
        case INNER_NODE: {
            Position node_position = oct_node_get_position(octree, node);

            uint8_t child_location = 0;
            child_location |= object_position.x < node_position.x ? 0 : 0b001;
            child_location |= object_position.y < node_position.y ? 0 : 0b010;
            child_location |= object_position.z < node_position.z ? 0 : 0b100;

            if (((BranchNode*)node)->child_exists &
                (1u << child_location)) {
                return oct_leaf_node_find(
                    octree,
                    oct_node_get_child(octree, node->location_code,
                                       child_location),
                    object_position);
            }

            return oct_leaf_node_init(octree, node->location_code,
                                      child_location, ULLONG_MAX);

            break;
        }
        default: 
            // If there was an error with the node type exit switch and return NULL
            break;
    }

    return NULL;
}

LeafNode*
oct_leaf_node_split(Octree* octree, LeafNode* node)
{
    uint64_t object_index = node->object_index;
    uint64_t location_code = node->base.location_code;
    oct_leaf_node_free(octree, location_code);

    BranchNode* inner_node = oct_branch_node_init(octree, location_code);
    if (node == NULL) {
        return NULL;
    }

    LeafNode* new_child =
        oct_leaf_node_find(octree, (BaseNode*)inner_node,
                           octree->object_positions[object_index]);
    new_child->object_index = object_index;

    return new_child;
}

Position
oct_node_get_position(Octree* octree, BaseNode* node)
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
oct_node_get_tree_depth(Octree* octree, const BaseNode* node)
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

BaseNode*
oct_node_get_parent(Octree* octree, BaseNode* node)
{
    uint64_t location_code_parent = node->location_code >> 3;
    return oct_node_lookup(octree, location_code_parent);
}

BaseNode*
oct_node_get_child(Octree* octree, uint64_t location_code,
                   uint8_t child_location)
{
    uint64_t child_location_code = (location_code << 3) | child_location;
    return oct_node_lookup(octree, child_location_code);
}

BaseNode*
oct_node_lookup(Octree* octree, uint64_t location_code)
{
    return unordered_map_get(octree->nodes, &location_code);
}

size_t 
oct_octree_get_size(Octree* octree)
{
    return octree->size;
}

size_t 
oct_octree_get_leaf_count(Octree* octree)
{
    return octree->leaf_count;
}

size_t 
oct_octree_get_inner_count(Octree* octree)
{
    return octree->inner_count;
}

// TEST CASE
void
oct_visit_all(Octree* octree, BaseNode* node)
{
    for (int i = 0; i < 8; i++) {
        if (node->type == INNER_NODE) {
            if (((BranchNode*)node)->child_exists & (1 << i)) {
                BaseNode* child =
                    oct_node_get_child(octree, node->location_code, i);
                oct_visit_all(octree, child);
            }
        } else {
            /* printf("%p\n", node); */
        }
    }
}
