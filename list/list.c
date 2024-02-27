#include "list.h"

typedef struct {
    void * val;
    void * xor;
} node_s;

struct list_s {
    void (* drop)(void *);
    node_s * head;
    node_s * tail;
    size_t len;
    node_s * lPtr;
    node_s * mPtr;
    node_s * rPtr;
    size_t idx;
};

list_s * listMake(void (* drop)(void *))
{
    list_s * const refs = (list_s *)calloc(1, sizeof(list_s));
    if ( NULL != refs )
    {
        refs->drop = drop;
    }
    return refs;
}
void listFree(void * refs)
{
    listClear(refs);
    free(refs);
}
list_s * listClear(list_s * refs)
{
    while ( listLength(refs) > 0 )
    {
        listRemove(refs, 0);
    }
}
size_t listLength(list_s * refs)
{
    return ( NULL == refs ) ? ( 0 ) : ( refs->len ) ;
}
void * listAccess(list_s * refs, size_t idx)
{
/* local var */
    node_s * temp = NULL;

/* error pre-chk */
    if ( NULL == refs )
    {
        return NULL;
    }
    if ( idx >= listLength(refs) )
    {
        return NULL;
    }

/* logic main body */
    if ( idx == 0 )
    {
        refs->lPtr = NULL;
        refs->mPtr = refs->head;
        refs->rPtr = (node_s *)refs->mPtr->xor;
        refs->idx = idx;
        return refs->head->val;
    }

    if ( idx == listLength(refs) - 1 )
    {
        refs->rPtr = NULL;
        refs->mPtr = refs->tail;
        refs->lPtr = (node_s *)refs->mPtr->xor;
        refs->idx = idx;
        return refs->tail->val;
    }

    while ( idx > refs->idx )
    {
        temp = refs->lPtr;
        refs->lPtr = refs->mPtr;
        refs->mPtr = (node_s *)( (size_t)( refs->mPtr->xor ) ^ (size_t)( temp ) ) ;
        refs->rPtr = (node_s *)( (size_t)( refs->mPtr->xor ) ^ (size_t)( refs->lPtr ) ) ;
        refs->idx++;
    }

    while ( idx < refs->idx )
    {
        temp = refs->rPtr;
        refs->rPtr = refs->mPtr;
        refs->mPtr = (node_s *)( (size_t)( refs->mPtr->xor ) ^ (size_t)( temp ) ) ;
        refs->lPtr = (node_s *)( (size_t)( refs->mPtr->xor ) ^ (size_t)( refs->rPtr ) ) ;
        refs->idx--;
    }

    return refs->mPtr->val;
}
list_s * listInsert(list_s * refs, size_t idx, void * val)
{
/* local var */
    node_s * temp = NULL;

/* error pre-chk */
    if ( NULL == refs )
    {
        goto __error;
    }

/* logic main body */
    temp = (node_s *)calloc(1, sizeof(node_s));
    if ( NULL != temp )
    {
        temp->val = val;
        temp->xor = NULL;
    }
    else
    {
        goto __error;
    }

    if ( 0 == listLength(refs) )
    {
        refs->lPtr = refs->mPtr = NULL;
        refs->mPtr = refs->head = refs->tail = temp;
        refs->idx = 0;
    }
    else if ( idx == 0 )
    {
        refs->head->xor = (void *)( (size_t)( refs->head->xor ) ^ (size_t)( temp ) ) ;
        temp->xor = (void *)refs->head;
        refs->lPtr = NULL;
        refs->mPtr = temp;
        refs->rPtr = temp->xor;
        refs->idx = 0;
    }
    else if ( idx >= listLength(refs) )
    {
        refs->tail->xor = (void *)( (size_t)( refs->tail->xor ) ^ (size_t)( temp ) ) ;
        temp->xor = (void *)refs->tail;
        refs->rPtr = NULL;
        refs->mPtr = temp;
        refs->lPtr = temp->xor;
        refs->idx = listLength(refs);
    }
    else if ( NULL != listAccess(refs, idx) )
    {
        refs->lPtr->xor = (void *)( (size_t)( refs->lPtr->xor ) ^ (size_t)( refs->mPtr ) ^ (size_t)( temp ) );
        refs->mPtr->xor = (void *)( (size_t)( refs->rPtr ) ^ (size_t)( temp ) ) ;
        temp->xor = (void *)( (size_t)( refs->lPtr ) ^ (size_t)( refs->mPtr ) ) ;
        refs->rPtr = refs->mPtr;
        refs->mPtr = temp;
        refs->idx = idx;
    }
    else
    {
        goto __error;
    }

    refs->len++;
    return refs;

__error:
    if ( NULL != temp )
    {
        free(temp);
    }
    return NULL;
}
list_s * listRemove(list_s * refs, size_t idx)
{
/* local var */
    node_s * temp = NULL;

/* logic main body */
    if ( idx >= listLength(refs) )
    {
        return refs;
    }
    else
    {
        temp = refs->mPtr;
    }

    if ( 1 == listLength(refs) )
    {
        refs->head = refs->tail = refs->lPtr = refs->mPtr = refs->rPtr = NULL;
    }
    else if ( idx == 0 )
    {
        refs->head = (node_s *)refs->head->xor;
        refs->head->xor = (node_s *)( (size_t)( refs->head->xor ) ^ (size_t)( temp ) ) ;
        refs->lPtr = NULL;
        refs->mPtr = refs->head;
        refs->rPtr = (node_s *)refs->mPtr->xor;
        refs->idx = idx;
    }
    else if ( idx + 1 == listLength(refs) )
    {
        refs->tail = (node_s *)refs->tail->xor;
        refs->tail->xor = (node_s *)( (size_t)( refs->tail->xor ) ^ (size_t)( temp ) ) ;
        refs->rPtr = NULL;
        refs->mPtr = refs->tail;
        refs->lPtr = (node_s *)refs->mPtr->xor;
        refs->idx = idx;
    }
    else
    {
        refs->lPtr->xor = (node_s *)( (size_t)( refs->lPtr->xor ) ^ (size_t)( temp ) ^ (size_t)( refs->rPtr ) ) ;
        refs->rPtr->xor = (node_s *)( (size_t)( refs->rPtr->xor ) ^ (size_t)( temp ) ^ (size_t)( refs->lPtr ) ) ;
        refs->mPtr = refs->rPtr;
        refs->rPtr = (node_s *)( (size_t)( refs->mPtr->xor ) ^ (size_t)( refs->lPtr ) ) ;
        refs->idx = idx;
    }

    if ( NULL != refs->drop )
    {
        refs->drop(temp->val);
    }
    free(temp);
    refs->len--;
    return refs;
}
list_s * listChange(list_s * refs, size_t idx, void * val)
{
    void * temp = NULL;

    if ( idx >= listLength(refs) )
    {
        if ( refs != listInsert(refs, idx, val) )
        {
            temp = val;
        }
    }
    else 
    {
        temp = listAccess(refs, idx);
        refs->mPtr->val = val;
    }

    if ( NULL != refs->drop )
    {
        refs->drop(temp);
    }
    return refs;
}
list_s * listReverse(list_s * refs)
{
    if ( listLength(refs) > 1 )
    {
        refs->lPtr = NULL;
        refs->mPtr = refs->tail;
        refs->tail = refs->head;
        refs->head = refs->mPtr;
        refs->rPtr = refs->mPtr->xor;
        refs->idx = 0;
    }

    return refs;
}
list_s * listQuickSort(list_s * refs, int (* cmp)(void *, void *))
{
    // TODO
    (void *)cmp;
    return refs;
}
