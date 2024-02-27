#ifndef __MYTH_EPIC_LIB_TREE
#define __MYTH_EPIC_LIB_TREE

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>

typedef struct tree_s tree_s;

typedef struct {
    const char * const key;
    const void * const val;
} tree_iterator_s;

tree_s * treeMake();
void treeFree(void * refs);
tree_s * treeClear(tree_s * refs);
size_t treeSize(tree_s * refs);
void * treeAccess(tree_s * refs, char * key);
tree_s * treeRemove(tree_s * refs, char * key);
tree_s * treeChange(tree_s * refs, char * key, void * val);
tree_iterator_s * treeGetRoot(tree_s * refs);
tree_iterator_s * treeGetNext(tree_s * refs, tree_iterator_s * it);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MYTH_EPIC_LIB_TREE */
