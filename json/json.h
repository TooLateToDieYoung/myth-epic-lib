#ifndef __MYTH_EPIC_LIB_JSON
#define __MYTH_EPIC_LIB_JSON

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum { JErr = -1, JNul, JBoo, JInt, JFlt, JStr, JArr, JObj } json_e;

typedef struct json_s json_s;

size_t jsonStringify(json_s * refs, char * buffer, size_t size);

json_s * jsonParseFromFile(char * filename);
json_s * jsonParseByString(const char * const string, const char ** endptr);

FILE * jsonDump(json_s * refs, FILE * stream);

json_e jsonType(json_s * refs);

json_s * jsonNulMake();
json_s * jsonBooMake(bool data);
json_s * jsonIntMake(long data);
json_s * jsonFltMake(double data);
json_s * jsonStrMake(char * data);
json_s * jsonArrMake();
json_s * jsonObjMake();

void jsonFree(void * refs);

json_s * jsonBooSet(json_s * refs, bool data);
json_s * jsonIntSet(json_s * refs, long data);
json_s * jsonFltSet(json_s * refs, double data);
json_s * jsonStrSet(json_s * refs, char * data);

bool jsonBooGet(json_s * refs);
long jsonIntGet(json_s * refs);
double jsonFltGet(json_s * refs);
char * jsonStrGet(json_s * refs);

json_s * jsonArrInsert(json_s * refs, size_t idx, json_s * val);
json_s * jsonArrAccess(json_s * refs, size_t idx);
json_s * jsonArrRemove(json_s * refs, size_t idx);
json_s * jsonArrChange(json_s * refs, size_t idx, json_s * val);
size_t jsonArrLength(json_s * refs);

json_s * jsonObjAccess(json_s * refs, char * key);
json_s * jsonObjRemove(json_s * refs, char * key);
json_s * jsonObjChange(json_s * refs, char * key, json_s * val);
size_t jsonObjSize(json_s * refs);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MYTH_EPIC_LIB_JSON */
