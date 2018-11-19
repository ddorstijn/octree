#ifndef OCTREE_H
#define OCTREE_H

#if defined(_MSC_VER)
#define OCTREE_API __declspec(dllexport)
#else
#define OCTREE_API
#endif

#include "unordered_map.h"
#include <stdint.h>

#define INNER_NODE 0
#define LEAF_NODE 1

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct _OctreeContainer
    {
        float* position;
        size_t size;
        size_t inner_count;
        size_t leaf_count;
        void* root_node;
        float** object_positions;
        unordered_map* nodes;
    } OctreeContainer;

    typedef struct _OctreeBaseNode
    {
        uint64_t location_code;
        uint8_t type;
    } OctreeBaseNode;

    typedef struct _OctreeLeafNode
    {
        OctreeBaseNode base;
        uint64_t object_index;
    } OctreeLeafNode;

    typedef struct _OctreeInnerNode
    {
        OctreeBaseNode base;
        uint8_t child_exists;
    } OctreeInnerNode;

    OCTREE_API OctreeContainer* oct_octree_init(float* position, size_t size);

    OCTREE_API void oct_octree_free(OctreeContainer* octree);
    /**
     * @brief Build the octree around the center position, split until all
     * object positions are divided into there own leafnode.
     *
     * @param center The center position of the octree
     * @param object_positions
     * @param count
     * @return root octree node
     */
    OCTREE_API void oct_octree_build(OctreeContainer* octree,
                                     float** object_positions,
                                     size_t object_count);

    OCTREE_API OctreeInnerNode* oct_node_init_inner(OctreeContainer* octree,
                                                    uint64_t location_code);

    OCTREE_API OctreeLeafNode* oct_node_init_leaf(OctreeContainer* octree,
                                                  uint64_t parent_location,
                                                  uint8_t child_location,
                                                  uint64_t object_index);

    OCTREE_API OctreeLeafNode* oct_find_leaf_node(OctreeContainer* octree,
                                                  OctreeBaseNode* node,
                                                  float* object_position);

    OCTREE_API OctreeLeafNode* oct_node_split_leaf_node(OctreeContainer* octree,
                                                        OctreeLeafNode* node);

    OCTREE_API float* oct_node_get_position(OctreeContainer* octree,
                                            OctreeBaseNode* node);

    /**
     * @brief Get the depth of a node
     *
     * @param node
     * @return size_t tree_depth
     */
    OCTREE_API size_t oct_node_get_tree_depth(OctreeContainer* octree,
                                              const OctreeBaseNode* node);

    /**
     * @brief Get the parent node from the node passed to the function
     *
     * @param node
     * @return OctreeNode* parent
     */
    OCTREE_API void* oct_node_get_parent(OctreeContainer* octree,
                                         OctreeBaseNode* node);

    OCTREE_API void* oct_node_get_child(OctreeContainer* octree,
                                        uint64_t location_code,
                                        uint8_t child_location);

    /**
     * @brief Find a node based on it's location code
     *
     * @param locCode
     * @return OctreeNode* node
     */
    OCTREE_API void* oct_node_lookup(OctreeContainer* octree,
                                     uint64_t location_code);

    /**
     * @brief visit all octree nodes
     *
     * @param node
     * @return void
     */
    OCTREE_API void oct_visit_all(OctreeContainer* octree,
                                  OctreeBaseNode* node);

#ifdef __cplusplus
}
#endif

#endif
