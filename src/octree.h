#ifndef OCTREE_H
#define OCTREE_H

#if defined(EXPORT_SHARED)
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
    /**
     * @brief Struct with x, y, z position for an object
     * 
     */
    typedef struct _Position 
    {
        float x;
        float y;
        float z;
    } Position;

    /**
     * @brief Thr basic container for the octree which holds the metadata.
     *
     */
    typedef struct _OctreeContainer
    {
        Position position;
        size_t size;
        size_t inner_count;
        size_t leaf_count;
        void* root_node;
        Position* object_positions;
        unordered_map* nodes;
    } OctreeContainer;

    /**
     * @brief The base node where the inner node and leave node derive from.
     *        This holds the type of node and its location.
     *
     *        Thr location code works by taking the parent and adding the index
     * from the current node. This way you get a long mask of the path that
     * hasss been taken to get to this node. The index works as the follows:
     *        front:       back:
     *        --- ---     --- ---
     *       | 2 | 3 |   | 6 | 7 |
     *       | - | - |   | - | - |
     *       | 0 | 1 |   | 4 | 5 |
     *        --- ---     --- ---
     */
    typedef struct _OctreeBaseNode
    {
        uint64_t location_code;
        uint8_t type;
    } OctreeBaseNode;

    /**
     * @brief The leaf node. This holds the object index of the object.
     *        The object index can be used by the usser to find the right
     *        object in hissss array.
     */
    typedef struct _OctreeLeafNode
    {
        OctreeBaseNode base;
        uint64_t object_index;
    } OctreeLeafNode;

    /**
     * @brief The inner node is a node that doesn't contain an object but
     * branches into smaller nodes. It has a child exists flag that where every
     * bit is set for the correlating child. This can be used t quickly check if
     * child nodess already exist.
     */
    typedef struct _OctreeInnerNode
    {
        OctreeBaseNode base;
        uint8_t child_exists;
    } OctreeInnerNode;

    size_t hash_func(void* key);
    bool equals_func(void* key1, void* key2);

    /**
     * @brief Allocate an octree with a root node.
     *
     * @param position The center of the octree
     * @param size The length from the center to one of the sides of the octree
     * @return OctreeContainer* octree This contains metadata for the octree
     */
    OCTREE_API OctreeContainer* oct_octree_init(Position position, size_t size);

    /**
     * @brief Dessstroy the octree and deallocate all the nodes.
     *        NOTE: you do have to destroy object positions yourself!
     *
     * @param octree The octree that has to be freed
     */
    OCTREE_API void oct_octree_free(OctreeContainer* octree);

    /**
     * @brief Split the octree until all the objects are in their own node.
     *
     * @param octree
     * @param object_positions An array of positions (x, y, z) float
     * @param object_count Number of objects
     */
    OCTREE_API void oct_octree_build(OctreeContainer* octree,
                                     Position* object_positions,
                                     size_t object_count);

    /**
     * @brief Init an inner node.
     *
     * @param octree
     * @param location_code Code that is used as a hash to store the node. This
     * also shows what path is taken through the octree to get there.
     * @return OctreeInnerNode* new_node The newly allocated note
     */
    OCTREE_API OctreeInnerNode* oct_node_init_inner(OctreeContainer* octree,
                                                    uint64_t location_code);

    /**
     * @brief Remove inner node
     * 
     * @param octree The octree the node is part of
     * @param location_code The location_code of the node to be removed 
     */
    OCTREE_API void oct_free_inner(OctreeContainer* octree, uint64_t location_code);

    /**
     * @brief Init a leaf node.
     *
     * @param octree
     * @param parent_location
     * @param child_location
     * @param object_index
     * @return OctreeLeafNode* new_node The newly allocated note
     */
    OCTREE_API OctreeLeafNode* oct_node_init_leaf(OctreeContainer* octree,
                                                  uint64_t parent_location,
                                                  uint8_t child_location,
                                                  uint64_t object_index);

    /**
     * @brief Remove leaf node
     * 
     * @param octree The octree the node is part of
     * @param location_code The location_code of the node to be removed 
     */
    OCTREE_API void oct_free_leaf(OctreeContainer* octree, uint64_t location_code);

    /**
     * @brief Find the first leaf node that could potentially hold thr object.
     * If this node was not created before it creates a new leaf node.
     *
     * @param octree
     * @param node
     * @param object_position
     * @return OctreeLeafNode* leaf_node best-suited leaf node to hold object
     */
    OCTREE_API OctreeLeafNode* oct_find_leaf_node(OctreeContainer* octree,
                                                  OctreeBaseNode* node,
                                                  Position object_position);

    /**
     * @brief Split a leaf node, change it to an inner node, then create a
     * child node to hold the object index.
     *
     * @param octree
     * @param node
     * @return OctreeleafNode* child_node The new child node that holds the
     * object index
     */
    OCTREE_API OctreeLeafNode* oct_node_split_leaf_node(OctreeContainer* octree,
                                                        OctreeLeafNode* node);

    /**
     * @brief Calculate the position of the node.
     *
     * @param octree
     * @param node
     * @return Position positiom Vector 3 of position (x, y, z)
     */
    OCTREE_API Position oct_node_get_position(OctreeContainer* octree,
                                              OctreeBaseNode* node);

    /**
     * @brief Get the depth of a node.
     *
     * @param octree
     * @param node
     * @return size_t tree_depth
     */
    OCTREE_API size_t oct_node_get_tree_depth(OctreeContainer* octree,
                                              const OctreeBaseNode* node);

    /**
     * @brief Get the parent node from the node passed to the function
     *
     * @param octree
     * @param node
     * @return OctreeBaseNode* parent_node
     */
    OCTREE_API OctreeBaseNode* oct_node_get_parent(OctreeContainer* octree,
                                                   OctreeBaseNode* node);

    /**
     * @brief Get a child node based of the location code of the parent and the
     * index of the child. Note: The index equalsss the last three digits of the
     * location code.
     *
     * @param octree
     * @param location_code
     * @param child_location
     * @return OctreeBaseNode* child_node
     */
    OCTREE_API OctreeBaseNode* oct_node_get_child(OctreeContainer* octree,
                                                  uint64_t location_code,
                                                  uint8_t child_location);

    /**
     * @brief Find a node based on it's location code
     *
     * @param octree
     * @param location_code
     * @return OctreeBaseNode* node Note: NULL if no node hass been found@
     */
    OCTREE_API OctreeBaseNode* oct_node_lookup(OctreeContainer* octree,
                                               uint64_t location_code);

    /**
     * @brief Get the ssize of the boundss of the octree
     * 
     * @param octree 
     * @return size_t octree_size 
     */
    OCTREE_API size_t oct_octree_get_size(OctreeContainer* octree);

    /**
     * @brief Get the amount of leaf nodes in the octree
     * 
     * @param octree 
     * @return size_t leaf_count  
     */
    OCTREE_API size_t oct_octree_get_leaf_count(OctreeContainer* octree);

    /**
     * @brief Get the amount of inner/branch nodes in the octree
     * 
     * @param octree 
     * @return size_t inner_count 
     */
    OCTREE_API size_t oct_octree_get_inner_count(OctreeContainer* octree);

    /**
     * @brief visit all octree nodes
     *
     * @param octree
     * @param node
     */
    OCTREE_API void oct_visit_all(OctreeContainer* octree,
                                  OctreeBaseNode* node);

#ifdef __cplusplus
}
#endif

#endif
