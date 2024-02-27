#include "tree.h"

#include <stdlib.h>
#include <string.h>

typedef struct node_s node_s;

struct node_s {
    char * key;
    void * val;
    node_s * lPtr;
    node_s * rPtr;
    node_s * pPtr;
    size_t height;
};

struct tree_s {
    void (* drop)(void *);
    node_s * root;
    size_t size;
    node_s * curr;
};

#define _MAX(a, b) ( ( (a) > (b) ) ? (a) : (b) )

static node_s * _treeTrySearch(tree_s * refs, char * key, node_s ** last)
{
    int ret = 0;
    node_s * temp = NULL;
    
    if ( NULL == refs )
    {
        goto __exit;
    }
    if ( NULL == key )
    {
        goto __exit;
    }

    if ( NULL != refs->curr )
    {
        ret = strcmp(key, refs->curr->key);
        if ( 0 == ret )
        {
            temp = refs->curr->pPtr;
            goto __exit;
        }
    }

    refs->curr = refs->root;
    while ( NULL != refs->curr ) 
    {
        ret = strcmp(key, refs->curr->key);
        if ( 0 == ret )
        {
            temp = refs->curr->pPtr;
            goto __exit;
        }
        else if ( 0 > ret )
        {
            temp = refs->curr;
            refs->curr = refs->curr->lPtr;
        }
        else /* if ( 0 < ref ) */
        {
            temp = refs->curr;
            refs->curr = refs->curr->rPtr;
        }
    }

__exit:
    if ( NULL != last )
    {
        *last = temp;
    }

    return refs->curr;
}
static node_s * _treeNodeHeight(node_s * refs)
{
    return ( NULL == refs ) ? ( 0 ) : ( refs->height ) ;
}
static node_s * _treeNodeTidyUp(node_s * refs)
{
    size_t lHeight = 0;
    size_t rHeight = 0;
    node_s * pPtr = NULL;
    node_s * cPtr = refs;

    if ( NULL == refs )
    {
        return NULL;
    }

    lHeight = _treeNodeHeight(refs->lPtr);
    rHeight = _treeNodeHeight(refs->rPtr);

    if ( lHeight > rHeight + 1 )
    {
        pPtr = refs->pPtr;
        cPtr = refs->lPtr;
        refs->pPtr = cPtr;
        refs->lPtr = cPtr->rPtr;
        cPtr->rPtr = refs;
    }

    if ( lHeight + 1 < rHeight )
    {
        pPtr = refs->pPtr;
        cPtr = refs->rPtr;
        refs->pPtr = cPtr;
        refs->rPtr = cPtr->lPtr;
        cPtr->lPtr = refs;
    }

    if ( NULL != pPtr )
    {
        if ( refs == pPtr->lPtr )
        {
            pPtr->lPtr = cPtr;
        }
        else
        {
            pPtr->rPtr = cPtr;
        }
    }

    if ( cPtr == refs )
    {
        cPtr->height = 1 + _MAX(lHeight, rHeight);
    }
    else
    {
        lHeight = _treeNodeHeight(refs->lPtr);
        rHeight = _treeNodeHeight(refs->rPtr);
        refs->height = 1 + _MAX(lHeight, rHeight);

        lHeight = _treeNodeHeight(cPtr->lPtr);
        rHeight = _treeNodeHeight(cPtr->rPtr);
        cPtr->height = 1 + _MAX(lHeight, rHeight);
    }

    return ( NULL == cPtr->pPtr ) ? ( cPtr ) : _treeNodeTidyUp(cPtr->pPtr) ;
}

tree_s * treeMake(void (* drop)(void *))
{
    tree_s * const refs = (tree_s *)calloc(1, sizeof(tree_s));
    if ( NULL != refs )
    {
        refs->drop = drop;
    }
    return refs;
}
void treeFree(void * refs)
{
    treeClear(refs);
    free(refs);
}
tree_s * treeClear(tree_s * refs)
{
    while ( treeSize(refs) > 0 )
    {
        treeRemove(refs, treeGetRoot(refs)->key);
    }
}
size_t treeSize(tree_s * refs)
{
    return ( NULL == refs ) ? ( 0 ) : ( refs->size ) ;
}
void * treeAccess(tree_s * refs, char * key)
{
    if ( NULL == refs )
    {
        return NULL;
    }

    refs->curr = _treeTrySearch(refs, key, NULL);
    return ( NULL == refs->curr ) ? ( NULL ) : ( refs->curr->val ) ;
}
tree_s * treeRemove(tree_s * refs, char * key)
{
    size_t lHeight = 0;
    size_t rHeight = 0;
    node_s * temp = NULL;
    node_s * next = NULL;

    if ( NULL == refs )
    {
        return NULL;
    }

    temp = _treeTrySearch(refs, key, NULL);
    if ( NULL != refs->curr )
    {
        free(temp->key);
        if ( NULL != refs->drop )
        {
            refs->drop(temp->val);
        }

        do {
            lHeight = _treeNodeHeight(temp->lPtr);
            rHeight = _treeNodeHeight(temp->rPtr);

            if ( 0 == lHeight && 0 == rHeight )
            {
                refs->curr = temp->pPtr;
                next = NULL;
            }
            else if ( 0 == lHeight )
            {
                refs->curr = temp->rPtr;
                next = refs->curr;
                break;
            }
            else if ( 0 == rHeight )
            {
                refs->curr = temp->lPtr;
                next = refs->curr;
                break;
            }
            else
            {
                temp = ( lHeight > rHeight ) ? ( refs->curr->lPtr ) : ( refs->curr->rPtr ) ;
                refs->curr->key = temp->key;
                refs->curr->val = temp->val;
            }
        } while ( 0 != lHeight && 0 != rHeight );

        if ( NULL != next )
        {
            next->pPtr = temp->pPtr;
        }

        if ( NULL != temp->pPtr )
        {
            if ( temp == temp->pPtr->lPtr )
            {
                temp->pPtr->lPtr = next;
            }
            else
            {
                temp->pPtr->rPtr = next;
            }
        }

        free(temp);
        refs->size--;
        refs->root = _treeNodeTidyUp(refs->curr);
    }

    return refs;
}
tree_s * treeChange(tree_s * refs, char * key, void * val)
{
    int ret = 0;
    node_s * last = NULL;
    size_t size = 0;
    
    if ( NULL == refs )
    {
        return NULL;
    }
    if ( NULL == key )
    {
        return NULL;
    }

    refs->curr = _treeTrySearch(refs, key, &last);
    if ( NULL != refs->curr )
    {
        if ( NULL != refs->drop )
        {
            refs->drop(refs->curr->val);
        }
        refs->curr->val = val;
    }
    else
    {
        refs->curr = (node_s *)calloc(1, sizeof(node_s));
        if ( NULL == refs->curr )
        {
            goto __error;
        }

        size = 1 + strlen(key);
        refs->curr->key = (char *)malloc(size);
        if ( NULL == refs->curr->key )
        {
            goto __error;
        }
        memcpy(refs->curr->key, key, size);

        refs->curr->val = val;
        refs->curr->height = 1;
        refs->curr->pPtr = last;
        ret = strcmp(key, last->key);
        if ( 0 == ret )
        {
            goto __error;
        }
        else if ( 0 > ret )
        {
            last->lPtr = refs->curr;
        }
        else /* if ( 0 < ref ) */
        {
            last->rPtr = refs->curr;
        }

        refs->size++;
        refs->root = _treeNodeTidyUp(refs->curr);
    }

    return refs;

__error:
    if ( NULL != refs->drop )
    {
        refs->drop(val);
    }

    if ( NULL != refs->curr )
    {
        if ( NULL != refs->curr->key )
        {
            free(refs->curr->key);
        }
        free(refs->curr);
    }

    refs->curr = last;
    return NULL;
}
tree_iterator_s * treeGetRoot(tree_s * refs)
{
    if ( NULL == refs )
    {
        return NULL;
    }
    
    refs->curr = refs->root;
    return (tree_iterator_s *)refs->curr;
}
tree_iterator_s * treeGetNext(tree_s * refs, tree_iterator_s * it)
{
    if ( NULL == refs )
    {
        return NULL;
    }
    if ( NULL == it )
    {
        return NULL;
    }

    refs->curr = (node_s *)it;
    if ( NULL != refs->curr->lPtr )
    {
        refs->curr = refs->curr->lPtr;
        goto __exit;
    }

    if ( NULL != refs->curr->rPtr )
    {
        refs->curr = refs->curr->rPtr;
        goto __exit;
    }

    if ( NULL != refs->curr->pPtr )
    {
        while ( refs->curr == refs->curr->pPtr->rPtr )
        {
            refs->curr = refs->curr->pPtr;
            if ( NULL == refs->curr->pPtr )
            {
                refs->curr = NULL;
                goto __exit;
            }
        }

        while ( NULL == refs->curr->pPtr->rPtr )
        {
            refs->curr = refs->curr->pPtr;
            if ( NULL == refs->curr->pPtr )
            {
                refs->curr = NULL;
                goto __exit;
            }
        }

        refs->curr = refs->curr->pPtr->rPtr;
        goto __exit;
    }

__exit:
    return (tree_iterator_s *)refs->curr;
}
