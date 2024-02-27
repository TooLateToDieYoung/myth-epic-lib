/* C89 Std. */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Myth Epic Lib. */
#include "json.h"
#include "../list/list.h"
#include "../tree/tree.h"

typedef union {
    double num;
    char * str;
    list_s * arr;
    tree_s * obj;
} json_u;

struct json_s {
    json_u data;
    json_e type;
};

static json_s * _jsonMake(json_e type);
static char * _jsonStrDuplicates(const char * const src);

/* public */
size_t jsonStringify(json_s * refs, char * buffer, size_t size);

json_s * jsonParseFromFile(char * filename);
json_s * jsonParseByString(const char * const string, const char ** endptr);

FILE * jsonDump(json_s * refs, FILE * stream);

json_e jsonType(json_s * refs)
{
    return ( NULL == refs ) ? ( JErr ) : ( refs->type ) ;
}

json_s * jsonNulMake()
{
    return _jsonMake(JNul);
}
json_s * jsonBooMake(bool data)
{
    json_s * const refs = _jsonMake(JBoo);
    if ( NULL != refs )
    {
        refs->data.num = (double)data;
    }
    return refs;
}
json_s * jsonIntMake(long data)
{
    json_s * const refs = _jsonMake(JInt);
    if ( NULL != refs )
    {
        refs->data.num = (double)data;
    }
    return refs;
}
json_s * jsonFltMake(double data)
{
    json_s * const refs = _jsonMake(JFlt);
    if ( NULL != refs )
    {
        refs->data.num = data;
    }
    return refs;
}
json_s * jsonStrMake(char * data)
{
    json_s * const refs = _jsonMake(JStr);
    if ( NULL != refs )
    {
        refs->data.str = _jsonStrDuplicates(data);
        if ( NULL == refs->data.str )
        {
            jsonFree(refs);
            return NULL;
        }
    }
    return refs;
}
json_s * jsonArrMake()
{
    json_s * const refs = _jsonMake(JArr);
    if ( NULL != refs )
    {
        refs->data.arr = listMake(jsonFree);
        if ( NULL == refs->data.arr )
        {
            jsonFree(refs);
            return NULL;
        }
    }
    return refs;
}
json_s * jsonObjMake()
{
    json_s * const refs = _jsonMake(JObj);
    if ( NULL != refs )
    {
        refs->data.obj = treeMake(jsonFree);
        if ( NULL == refs->data.obj )
        {
            jsonFree(refs);
            return NULL;
        }
    }
    return refs;
}

void jsonFree(void * refs)
{
    json_s * const temp = (json_s *)refs;
    if ( NULL != temp )
    {
        switch ( jsonType(temp) )
        {
            case JStr:
                free(temp->data.str);
                break;
            
            case JArr:
                listFree(temp->data.arr);
                break;

            case JObj:
                treeFree(temp->data.obj);
                break;
            
            default:
                break;
        }

        free(temp);
    }
}

json_s * jsonBooSet(json_s * refs, bool data)
{
    if ( JBoo == jsonType(refs) )
    {
        refs->data.num = (double)data;
    }
    return refs;
}
json_s * jsonIntSet(json_s * refs, long data)
{
    if ( JInt == jsonType(refs) )
    {
        refs->data.num = (double)data;
    }
    return refs;
}
json_s * jsonFltSet(json_s * refs, double data)
{
    if ( JFlt == jsonType(refs) )
    {
        refs->data.num = data;
    }
    return refs;
}
json_s * jsonStrSet(json_s * refs, char * data)
{
    size_t srcLen = 0;
    size_t dstLen = 0;
    char * temp = NULL;
    if ( JStr == jsonType(refs) )
    {
        srcLen = 1 + strlen(data);
        dstLen = 1 + strlen(refs->data.str);
        if ( dstLen >= srcLen )
        {
            memcpy(refs->data.str, data, srcLen);
        }
        else
        {
            temp = _jsonStrDuplicates(data);
            if ( NULL != temp )
            {
                free(refs->data.str);
                refs->data.str = data;
            }
        }
    }
    return refs;
}

bool jsonBooGet(json_s * refs);
long jsonIntGet(json_s * refs);
double jsonFltGet(json_s * refs);
char * jsonStrGet(json_s * refs);

json_s * jsonArrInsert(json_s * refs, size_t idx, json_s * val)
{
    return ( JArr != jsonType(refs) ) ? ( NULL ) : listInsert(refs->data.arr, idx, val) ;
}
json_s * jsonArrAccess(json_s * refs, size_t idx)
{
    return ( JArr != jsonType(refs) ) ? ( NULL ) : (json_s *)listAccess(refs->data.arr, idx) ;
}
json_s * jsonArrRemove(json_s * refs, size_t idx)
{
    return ( 
        JArr == jsonType(refs) &&
        refs->data.arr == listRemove(refs->data.arr, idx) 
    ) ? ( refs ) : ( NULL ) ;
}
json_s * jsonArrChange(json_s * refs, size_t idx, json_s * val)
{
    return ( JArr != jsonType(refs) ) ? ( NULL ) : listChange(refs->data.arr, idx, val) ;
}
size_t jsonArrLength(json_s * refs)
{
    return ( JArr != jsonType(refs) ) ? ( 0 ) : listLength(refs->data.arr) ;
}

json_s * jsonObjAccess(json_s * refs, char * key)
{
    return ( JObj != jsonType(refs) ) ? ( NULL ) : treeAccess(refs->data.obj, key) ;
}
json_s * jsonObjRemove(json_s * refs, char * key)
{
    return ( 
        JObj == jsonType(refs) && 
        refs->data.obj == treeRemove(refs->data.obj, key) 
    ) ? ( refs ) : ( NULL ) ;
}
json_s * jsonObjChange(json_s * refs, char * key, json_s * val)
{
    return ( JObj != jsonType(refs) ) ? ( NULL ) : treeChange(refs->data.obj, key, val) ;
}
size_t jsonObjSize(json_s * refs)
{
    return ( JObj != jsonType(refs) ) ? ( 0 ) : treeSize(refs->data.obj) ;
}

/* private */
static json_s * _jsonMake(json_e type)
{
    json_s * const refs = (json_s *)calloc(1, sizeof(json_s));
    if ( NULL != refs )
    {
        refs->type = type;
    }
    return refs;
}
static char * _jsonStrDuplicates(const char * const src)
{
    size_t len = 0;
    char * tmp = NULL;

    tmp = strchr(src, '"');
    len = ( NULL != tmp ) ? ( tmp - src ) : strlen(src) ;
    tmp = (char *)calloc(1 + len, sizeof(char));
    memcpy(tmp, src, len);

    return tmp;
}
