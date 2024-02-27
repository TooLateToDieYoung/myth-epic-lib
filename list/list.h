#ifndef __MYTH_EPIC_LIB_LIST
#define __MYTH_EPIC_LIB_LIST

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>

typedef struct list_s list_s;

list_s * listMake(void (* drop)(void *));
void listFree(void * refs);
list_s * listClear(list_s * refs);
size_t listLength(list_s * refs);
void * listAccess(list_s * refs, size_t idx);
list_s * listRemove(list_s * refs, size_t idx);
list_s * listChange(list_s * refs, size_t idx, void * val);
list_s * listInsert(list_s * refs, size_t idx, void * val);
list_s * listReverse(list_s * refs);
list_s * listQuickSort(list_s * refs, int (* cmp)(void *, void *));

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MYTH_EPIC_LIB_LIST */
