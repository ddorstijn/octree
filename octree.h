#ifndef OCTREE_H
#define OCTREE_H

#include <stdint.h>
#include "unordered_map.h"

#define OCTREE_API __declspec(dllexport) 

#ifdef	__cplusplus
extern "C" {
#endif

typedef OCTREE_API struct _OctreeNode {
    uint64_t    object_index;
    uint64_t    location_code;
    uint8_t     child_exists;
} OctreeNode;

typedef OCTREE_API struct _OctreeContainer
{
    float*          center_position;
    size_t          total_node_count;
    unordered_map*  nodes; 
} OctreeContainer;

/**
 * @brief Build the octree around the center position, split until all object positions
 *        are divided into there own leafnode.
 * 
 * @param center The center position of the octree
 * @param object_positions 
 * @param count 
 * @return root octree node 
 */
OCTREE_API void oct_octree_build(OctreeContainer* octree, float** object_positions, size_t object_count); 

OCTREE_API OctreeContainer* oct_octree_init(float* center_position);

OCTREE_API OctreeNode* oct_node_init(uint64_t object_index, uint64_t location_code);

OCTREE_API size_t oct_node_get_position(OctreeNode* node);

/**
 * @brief Get the depth of a node
 * 
 * @param node 
 * @return size_t tree_depth 
 */
OCTREE_API size_t oct_node_get_tree_depth(OctreeContainer* octree, const OctreeNode* node);

/**
 * @brief Get the parent node from the node passed to the function
 * 
 * @param node 
 * @return OctreeNode* parent
 */
OCTREE_API OctreeNode* oct_node_get_parent(OctreeContainer* octree, OctreeNode* node);

/**
 * @brief Find a node based on it's location code
 * 
 * @param locCode 
 * @return OctreeNode* node
 */
OCTREE_API OctreeNode* oct_node_lookup(OctreeContainer* octree, uint64_t location_code);

/**
 * @brief visit all octree nodes
 * 
 * @param node 
 * @return void
 */
OCTREE_API void oct_visit_all(OctreeContainer* octree, OctreeNode* node);

#ifdef	__cplusplus
}
#endif

#endif