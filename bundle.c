
#pragma region OPTION_H
#ifndef OPTION_H
#define OPTION_H
/**
 * @file option.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module for defining and using Options made from pure macros 👌
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 *
 */
typedef enum __attribute__((packed)) _OptionType
{
    NONE,
    SOME
} OptionType;

#define option_type(T)                                                                                                 \
    typedef struct __attribute__((packed)) _##T##Option                                                                \
    {                                                                                                                  \
        OptionType type;                                                                                               \
        T value;                                                                                                       \
    } T##Option;                                                                                                       \
    T##Option no_##T()                                                                                                 \
    {                                                                                                                  \
        return (T##Option){.type = NONE};                                                                              \
    }                                                                                                                  \
    T##Option some_##T(T value)                                                                                        \
    {                                                                                                                  \
        return (T##Option){.type = SOME, .value = value};                                                              \
    }

#define is_some(option) ((option.type) == SOME)
#define is_none(option) ((option.type) == NONE)

#endif

#pragma endregion OPTION_H

#pragma region FAIL_H
#ifndef FAIL_H
#define FAIL_H

#include <stdbool.h>
#include <stdlib.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include <stdio.h>
#include <stdarg.h>

/**
 * @brief The fail-module.
 * 
 * @file fail.h
 * @author WingCorp (https://github.com/WingCorp)
 * @date 2018-06-19
 */

/**
 * @brief Exit the program with an error value.
 */
void fail();

#define FAIL_RED "\x1b[31m"
#define FAIL_RESET "\x1b[0m"

void trace_stack();

#define failwith(message)            \
    do {                             \
        fprintf(stderr, FAIL_RED);   \
        fprintf(stderr, message);    \
        fprintf(stderr, FAIL_RESET); \
        trace_stack();               \
        exit(EXIT_FAILURE);          \
    } while (0)

/**
 * @brief Print the error cause and exit the program with an error value.
 * 
 * @param cause to print before exiting. Use this like you would use printf.
 * @param VAR_ARGS, whatever you want it to say in your failure output.
 */
#define failwithf(format, ...)                      \
    do {                                            \
        fprintf(stderr, FAIL_RED);                  \
        fprintf(stderr, format, ##__VA_ARGS__);     \
        fprintf(stderr, FAIL_RESET);                \
        trace_stack();                              \
        exit(EXIT_FAILURE);                         \
    } while(0)                          

#endif

void* callstack[1024];
void trace_stack()
{
    #ifdef __linux__
    int frames = backtrace(callstack, 1024);
    char** strs = backtrace_symbols(callstack, frames);
    int i;
    fprintf(stderr, FAIL_RED);
    for (i = 0; i < frames; i++)
    {
        fprintf(stderr, "%s\n", strs[i]);
    }
    fprintf(stderr, FAIL_RESET);
    free(strs);
    #else
    fprintf(stderr, FAIL_RED);
    fprintf(stderr, "No backtrace available on mingw :<\n");
    fprintf(stderr, FAIL_RESET);
    #endif
}

void fail()
{
    trace_stack();
    exit(EXIT_FAILURE);
}
#pragma endregion FAIL_H

#pragma region DEFS_H
#ifndef DEFS_H
#define DEFS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux
#include <unistd.h>
#endif

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;


typedef float f32;
typedef double f64;

#define eps 0.000001f
#define max(a, b) (a) > (b) ? a : b
#define min(a, b) (a) < (b) ? a : b
#define fcmp(a, b) (a - b > eps) ? 1 : (b - a > eps) ? -1 : 0
#define feq(a, b) (fcmp(a, b) == 0)

#endif

#pragma endregion DEFS_H

#pragma region HASHMAP_H
#ifndef HASHMAP_H
#define HASHMAP_H
/**
 * @file hashmap.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A simple hashmap made by chatgpt :)
 * @version 0.1
 * @date 2023-08-27
 *
 * @copyright WingCorp (c) 2023
 *
 */

#define hashmap_type(K, V, hashfunc, keycompfunc)                                                                      \
    typedef struct _##K##V##HashmapEntry K##V##HashmapEntry;                                                           \
    typedef struct _##K##V##HashmapEntry                                                                               \
    {                                                                                                                  \
        K key;                                                                                                         \
        V value;                                                                                                       \
        K##V##HashmapEntry *next;                                                                                      \
    } K##V##HashmapEntry;                                                                                              \
                                                                                                                       \
    typedef struct _##K##V##Hashmap                                                                                    \
    {                                                                                                                  \
        K##V##HashmapEntry **buckets;                                                                                  \
        u32 size;                                                                                                      \
    } K##V##Hashmap;                                                                                                   \
                                                                                                                       \
    K##V##Hashmap *new_##K##V##Hashmap(u32 capacity)                                                                   \
    {                                                                                                                  \
        K##V##Hashmap *map = malloc(sizeof(K##V##Hashmap));                                                            \
        if (map == NULL)                                                                                               \
        {                                                                                                              \
            failwith("Failed to allocate memory for hashmap\n");                                                       \
        }                                                                                                              \
        map->buckets = calloc(capacity, sizeof(K##V##HashmapEntry *));                                                 \
        map->size = 0;                                                                                                 \
        return map;                                                                                                    \
    }                                                                                                                  \
                                                                                                                       \
    void K##V##Hashmap_put(K##V##Hashmap *map, K key, V value)                                                         \
    {                                                                                                                  \
        size_t index = hashfunc(key) % map->size;                                                                      \
        K##V##HashmapEntry *newEntry = malloc(sizeof(K##V##HashmapEntry));                                             \
        if (newEntry == NULL)                                                                                          \
        {                                                                                                              \
            failwith("Failed to allocate memory for hashmap entry\n");                                                 \
        }                                                                                                              \
        newEntry->key = key;                                                                                           \
        newEntry->value = value;                                                                                       \
        newEntry->next = map->buckets[index];                                                                          \
        map->buckets[index] = newEntry;                                                                                \
        map->size++;                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    V K##V##Hashmap_get(K##V##Hashmap *map, K key)                                                                     \
    {                                                                                                                  \
        size_t index = hashfunc(key) % map->size;                                                                      \
        K##V##HashmapEntry *entry = map->buckets[index];                                                               \
        while (entry != NULL)                                                                                          \
        {                                                                                                              \
            if (keycompfunc(entry->key, key) == 0)                                                                     \
            {                                                                                                          \
                return entry->value;                                                                                   \
            }                                                                                                          \
            entry = entry->next;                                                                                       \
        }                                                                                                              \
        failwith("Entry not found!\n");                                                                                \
        return (V){0};                                                                                                 \
    }                                                                                                                  \
    void destroy_##K##V##Hashmap(K##V##Hashmap *map)                                                                   \
    {                                                                                                                  \
        for (u32 i = 0; i < map->size; i++)                                                                            \
        {                                                                                                              \
            K##V##HashmapEntry *entry = map->buckets[i];                                                               \
            while (entry != NULL)                                                                                      \
            {                                                                                                          \
                K##V##HashmapEntry *temp = entry;                                                                      \
                entry = entry->next;                                                                                   \
                free(temp);                                                                                            \
            }                                                                                                          \
        }                                                                                                              \
        free(map->buckets);                                                                                            \
        free(map);                                                                                                     \
    }

#endif


#pragma endregion HASHMAP_H

#pragma region VEC3_H
#ifndef VEC3_H
#define VEC3_H
/**
 * @file vec3.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A vector module. Yay.
 * @version 0.1
 * @date 2023-08-21
 *
 * @copyright WingCorp (c) 2023
 * 
 */


typedef struct __attribute__((packed)) _Vec3
{
    f32 x;
    f32 y;
    f32 z;
} Vec3;

Vec3 vec3(f32 x, f32 y, f32 z);

f32 mag(Vec3 v);

Vec3 norm(Vec3 v);

f32 dist(Vec3 a, Vec3 b);

f32 dot(Vec3 a, Vec3 b);

Vec3 smul(Vec3 v, f32 s);

Vec3 vmul(Vec3 a, Vec3 b);

Vec3 vadd(Vec3 a, Vec3 b);

Vec3 vsub(Vec3 a, Vec3 b);

Vec3 cross(Vec3 a, Vec3 b);

Vec3 vabs(Vec3 v);

#endif
#include <math.h>

#define sq(expr) (expr) * (expr)

Vec3 vec3(const f32 x, const f32 y, const f32 z) {
    return (Vec3){.x = x, .y = y, .z = z};
}

f32 mag(const Vec3 v) {
    return sqrtf(sq(v.x) + sq(v.y) + sq(v.z));
}

Vec3 norm(const Vec3 v) {
    const f32 m = mag(v);
    return vec3(v.x / m, v.y / m, v.z / m);
}

f32 dist(const Vec3 a, const Vec3 b) {
    return sqrtf(sq(b.x - a.x) + sq(b.y - a.y) + sq(b.z - a.z));
}

f32 dot(const Vec3 a, const Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 smul(Vec3 v, f32 s) {
    return vec3(v.x * s, v.y * s, v.z * s);
}

Vec3 vmul(Vec3 a, Vec3 b) {
    return vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vec3 vadd(Vec3 a, Vec3 b) {
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 vsub(Vec3 a, Vec3 b) {
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 cross(Vec3 a, Vec3 b) {
    return vec3(
        a.y * b.z - a.z * b.y, 
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

Vec3 vabs(Vec3 v) {
    return vec3(fabsf(v.x), fabsf(v.y), fabsf(v.z));
}

#pragma endregion VEC3_H

#pragma region CJSON_H
/*
  Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef cJSON__h
#define cJSON__h

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__

/* When compiling for windows, we specify a specific calling convention to avoid issues where we are being called from a project with a different default calling convention.  For windows you have 3 define options:

CJSON_HIDE_SYMBOLS - Define this in the case where you don't want to ever dllexport symbols
CJSON_EXPORT_SYMBOLS - Define this on library build when you want to dllexport symbols (default)
CJSON_IMPORT_SYMBOLS - Define this if you want to dllimport symbol

For *nix builds that support visibility attribute, you can define similar behavior by

setting default visibility to hidden by adding
-fvisibility=hidden (for gcc)
or
-xldscope=hidden (for sun cc)
to CFLAGS

then using the CJSON_API_VISIBILITY flag to "export" the same symbols the way CJSON_EXPORT_SYMBOLS does

*/

#define CJSON_CDECL __cdecl
#define CJSON_STDCALL __stdcall

/* export symbols by default, this is necessary for copy pasting the C and header file */
#if !defined(CJSON_HIDE_SYMBOLS) && !defined(CJSON_IMPORT_SYMBOLS) && !defined(CJSON_EXPORT_SYMBOLS)
#define CJSON_EXPORT_SYMBOLS
#endif

#if defined(CJSON_HIDE_SYMBOLS)
#define CJSON_PUBLIC(type)   type CJSON_STDCALL
#elif defined(CJSON_EXPORT_SYMBOLS)
#define CJSON_PUBLIC(type)   __declspec(dllexport) type CJSON_STDCALL
#elif defined(CJSON_IMPORT_SYMBOLS)
#define CJSON_PUBLIC(type)   __declspec(dllimport) type CJSON_STDCALL
#endif
#else /* !__WINDOWS__ */
#define CJSON_CDECL
#define CJSON_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined (__SUNPRO_C)) && defined(CJSON_API_VISIBILITY)
#define CJSON_PUBLIC(type)   __attribute__((visibility("default"))) type
#else
#define CJSON_PUBLIC(type) type
#endif
#endif

/* project version */
#define CJSON_VERSION_MAJOR 1
#define CJSON_VERSION_MINOR 7
#define CJSON_VERSION_PATCH 16

#include <stddef.h>

/* cJSON Types: */
#define cJSON_Invalid (0)
#define cJSON_False  (1 << 0)
#define cJSON_True   (1 << 1)
#define cJSON_NULL   (1 << 2)
#define cJSON_Number (1 << 3)
#define cJSON_String (1 << 4)
#define cJSON_Array  (1 << 5)
#define cJSON_Object (1 << 6)
#define cJSON_Raw    (1 << 7) /* raw json */

#define cJSON_IsReference 256
#define cJSON_StringIsConst 512

/* The cJSON structure: */
typedef struct cJSON
{
    /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSON *next;
    struct cJSON *prev;
    /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
    struct cJSON *child;

    /* The type of the item, as above. */
    int type;

    /* The item's string, if type==cJSON_String  and type == cJSON_Raw */
    char *valuestring;
    /* writing to valueint is DEPRECATED, use cJSON_SetNumberValue instead */
    int valueint;
    /* The item's number, if type==cJSON_Number */
    double valuedouble;

    /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
    char *string;
} cJSON;

typedef struct cJSON_Hooks
{
      /* malloc/free are CDECL on Windows regardless of the default calling convention of the compiler, so ensure the hooks allow passing those functions directly. */
      void *(CJSON_CDECL *malloc_fn)(size_t sz);
      void (CJSON_CDECL *free_fn)(void *ptr);
} cJSON_Hooks;

typedef int cJSON_bool;

/* Limits how deeply nested arrays/objects can be before cJSON rejects to parse them.
 * This is to prevent stack overflows. */
#ifndef CJSON_NESTING_LIMIT
#define CJSON_NESTING_LIMIT 1000
#endif

/* returns the version of cJSON as a string */
CJSON_PUBLIC(const char*) cJSON_Version(void);

/* Supply malloc, realloc and free functions to cJSON */
CJSON_PUBLIC(void) cJSON_InitHooks(cJSON_Hooks* hooks);

/* Memory Management: the caller is always responsible to free the results from all variants of cJSON_Parse (with cJSON_Delete) and cJSON_Print (with stdlib free, cJSON_Hooks.free_fn, or cJSON_free as appropriate). The exception is cJSON_PrintPreallocated, where the caller has full responsibility of the buffer. */
/* Supply a block of JSON, and this returns a cJSON object you can interrogate. */
CJSON_PUBLIC(cJSON *) cJSON_Parse(const char *value);
CJSON_PUBLIC(cJSON *) cJSON_ParseWithLength(const char *value, size_t buffer_length);
/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
/* If you supply a ptr in return_parse_end and parsing fails, then return_parse_end will contain a pointer to the error so will match cJSON_GetErrorPtr(). */
CJSON_PUBLIC(cJSON *) cJSON_ParseWithOpts(const char *value, const char **return_parse_end, cJSON_bool require_null_terminated);
CJSON_PUBLIC(cJSON *) cJSON_ParseWithLengthOpts(const char *value, size_t buffer_length, const char **return_parse_end, cJSON_bool require_null_terminated);

/* Render a cJSON entity to text for transfer/storage. */
CJSON_PUBLIC(char *) cJSON_Print(const cJSON *item);
/* Render a cJSON entity to text for transfer/storage without any formatting. */
CJSON_PUBLIC(char *) cJSON_PrintUnformatted(const cJSON *item);
/* Render a cJSON entity to text using a buffered strategy. prebuffer is a guess at the final size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
CJSON_PUBLIC(char *) cJSON_PrintBuffered(const cJSON *item, int prebuffer, cJSON_bool fmt);
/* Render a cJSON entity to text using a buffer already allocated in memory with given length. Returns 1 on success and 0 on failure. */
/* NOTE: cJSON is not always 100% accurate in estimating how much memory it will use, so to be safe allocate 5 bytes more than you actually need */
CJSON_PUBLIC(cJSON_bool) cJSON_PrintPreallocated(cJSON *item, char *buffer, const int length, const cJSON_bool format);
/* Delete a cJSON entity and all subentities. */
CJSON_PUBLIC(void) cJSON_Delete(cJSON *item);

/* Returns the number of items in an array (or object). */
CJSON_PUBLIC(int) cJSON_GetArraySize(const cJSON *array);
/* Retrieve item number "index" from array "array". Returns NULL if unsuccessful. */
CJSON_PUBLIC(cJSON *) cJSON_GetArrayItem(const cJSON *array, int index);
/* Get item "string" from object. Case insensitive. */
CJSON_PUBLIC(cJSON *) cJSON_GetObjectItem(const cJSON * const object, const char * const string);
CJSON_PUBLIC(cJSON *) cJSON_GetObjectItemCaseSensitive(const cJSON * const object, const char * const string);
CJSON_PUBLIC(cJSON_bool) cJSON_HasObjectItem(const cJSON *object, const char *string);
/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds. */
CJSON_PUBLIC(const char *) cJSON_GetErrorPtr(void);

/* Check item type and return its value */
CJSON_PUBLIC(char *) cJSON_GetStringValue(const cJSON * const item);
CJSON_PUBLIC(double) cJSON_GetNumberValue(const cJSON * const item);

/* These functions check the type of an item */
CJSON_PUBLIC(cJSON_bool) cJSON_IsInvalid(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsFalse(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsTrue(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsBool(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsNull(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsNumber(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsString(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsArray(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsObject(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsRaw(const cJSON * const item);

/* These calls create a cJSON item of the appropriate type. */
CJSON_PUBLIC(cJSON *) cJSON_CreateNull(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateTrue(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateFalse(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateBool(cJSON_bool boolean);
CJSON_PUBLIC(cJSON *) cJSON_CreateNumber(double num);
CJSON_PUBLIC(cJSON *) cJSON_CreateString(const char *string);
/* raw json */
CJSON_PUBLIC(cJSON *) cJSON_CreateRaw(const char *raw);
CJSON_PUBLIC(cJSON *) cJSON_CreateArray(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateObject(void);

/* Create a string where valuestring references a string so
 * it will not be freed by cJSON_Delete */
CJSON_PUBLIC(cJSON *) cJSON_CreateStringReference(const char *string);
/* Create an object/array that only references it's elements so
 * they will not be freed by cJSON_Delete */
CJSON_PUBLIC(cJSON *) cJSON_CreateObjectReference(const cJSON *child);
CJSON_PUBLIC(cJSON *) cJSON_CreateArrayReference(const cJSON *child);

/* These utilities create an Array of count items.
 * The parameter count cannot be greater than the number of elements in the number array, otherwise array access will be out of bounds.*/
CJSON_PUBLIC(cJSON *) cJSON_CreateIntArray(const int *numbers, int count);
CJSON_PUBLIC(cJSON *) cJSON_CreateFloatArray(const float *numbers, int count);
CJSON_PUBLIC(cJSON *) cJSON_CreateDoubleArray(const double *numbers, int count);
CJSON_PUBLIC(cJSON *) cJSON_CreateStringArray(const char *const *strings, int count);

/* Append item to the specified array/object. */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToArray(cJSON *array, cJSON *item);
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item);
/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the cJSON object.
 * WARNING: When this function was used, make sure to always check that (item->type & cJSON_StringIsConst) is zero before
 * writing to `item->string` */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToObjectCS(cJSON *object, const char *string, cJSON *item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing cJSON to a new cJSON, but don't want to corrupt your existing cJSON. */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item);
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemReferenceToObject(cJSON *object, const char *string, cJSON *item);

/* Remove/Detach items from Arrays/Objects. */
CJSON_PUBLIC(cJSON *) cJSON_DetachItemViaPointer(cJSON *parent, cJSON * const item);
CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromArray(cJSON *array, int which);
CJSON_PUBLIC(void) cJSON_DeleteItemFromArray(cJSON *array, int which);
CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromObject(cJSON *object, const char *string);
CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromObjectCaseSensitive(cJSON *object, const char *string);
CJSON_PUBLIC(void) cJSON_DeleteItemFromObject(cJSON *object, const char *string);
CJSON_PUBLIC(void) cJSON_DeleteItemFromObjectCaseSensitive(cJSON *object, const char *string);

/* Update array items. */
CJSON_PUBLIC(cJSON_bool) cJSON_InsertItemInArray(cJSON *array, int which, cJSON *newitem); /* Shifts pre-existing items to the right. */
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemViaPointer(cJSON * const parent, cJSON * const item, cJSON * replacement);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInArray(cJSON *array, int which, cJSON *newitem);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInObject(cJSON *object,const char *string,cJSON *newitem);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInObjectCaseSensitive(cJSON *object,const char *string,cJSON *newitem);

/* Duplicate a cJSON item */
CJSON_PUBLIC(cJSON *) cJSON_Duplicate(const cJSON *item, cJSON_bool recurse);
/* Duplicate will create a new, identical cJSON item to the one you pass, in new memory that will
 * need to be released. With recurse!=0, it will duplicate any children connected to the item.
 * The item->next and ->prev pointers are always zero on return from Duplicate. */
/* Recursively compare two cJSON items for equality. If either a or b is NULL or invalid, they will be considered unequal.
 * case_sensitive determines if object keys are treated case sensitive (1) or case insensitive (0) */
CJSON_PUBLIC(cJSON_bool) cJSON_Compare(const cJSON * const a, const cJSON * const b, const cJSON_bool case_sensitive);

/* Minify a strings, remove blank characters(such as ' ', '\t', '\r', '\n') from strings.
 * The input pointer json cannot point to a read-only address area, such as a string constant, 
 * but should point to a readable and writable address area. */
CJSON_PUBLIC(void) cJSON_Minify(char *json);

/* Helper functions for creating and adding items to an object at the same time.
 * They return the added item or NULL on failure. */
CJSON_PUBLIC(cJSON*) cJSON_AddNullToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddTrueToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddFalseToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddBoolToObject(cJSON * const object, const char * const name, const cJSON_bool boolean);
CJSON_PUBLIC(cJSON*) cJSON_AddNumberToObject(cJSON * const object, const char * const name, const double number);
CJSON_PUBLIC(cJSON*) cJSON_AddStringToObject(cJSON * const object, const char * const name, const char * const string);
CJSON_PUBLIC(cJSON*) cJSON_AddRawToObject(cJSON * const object, const char * const name, const char * const raw);
CJSON_PUBLIC(cJSON*) cJSON_AddObjectToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddArrayToObject(cJSON * const object, const char * const name);

/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define cJSON_SetIntValue(object, number) ((object) ? (object)->valueint = (object)->valuedouble = (number) : (number))
/* helper for the cJSON_SetNumberValue macro */
CJSON_PUBLIC(double) cJSON_SetNumberHelper(cJSON *object, double number);
#define cJSON_SetNumberValue(object, number) ((object != NULL) ? cJSON_SetNumberHelper(object, (double)number) : (number))
/* Change the valuestring of a cJSON_String object, only takes effect when type of object is cJSON_String */
CJSON_PUBLIC(char*) cJSON_SetValuestring(cJSON *object, const char *valuestring);

/* If the object is not a boolean type this does nothing and returns cJSON_Invalid else it returns the new type*/
#define cJSON_SetBoolValue(object, boolValue) ( \
    (object != NULL && ((object)->type & (cJSON_False|cJSON_True))) ? \
    (object)->type=((object)->type &(~(cJSON_False|cJSON_True)))|((boolValue)?cJSON_True:cJSON_False) : \
    cJSON_Invalid\
)

/* Macro for iterating over an array or object */
#define cJSON_ArrayForEach(element, array) for(element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

/* malloc/free objects using the malloc/free functions that have been set with cJSON_InitHooks */
CJSON_PUBLIC(void *) cJSON_malloc(size_t size);
CJSON_PUBLIC(void) cJSON_free(void *object);

#ifdef __cplusplus
}
#endif

#endif
/*
  Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

/* cJSON */
/* JSON parser in C. */

/* disable warnings about old C89 functions in MSVC */
#if !defined(_CRT_SECURE_NO_DEPRECATE) && defined(_MSC_VER)
#define _CRT_SECURE_NO_DEPRECATE
#endif

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif
#if defined(_MSC_VER)
#pragma warning (push)
/* disable warning about single line comments in system headers */
#pragma warning (disable : 4001)
#endif

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <float.h>

#ifdef ENABLE_LOCALES
#include <locale.h>
#endif

#if defined(_MSC_VER)
#pragma warning (pop)
#endif
#ifdef __GNUC__
#pragma GCC visibility pop
#endif


/* define our own boolean type */
#ifdef true
#undef true
#endif
#define true ((cJSON_bool)1)

#ifdef false
#undef false
#endif
#define false ((cJSON_bool)0)

/* define isnan and isinf for ANSI C, if in C99 or above, isnan and isinf has been defined in math.h */
#ifndef isinf
#define isinf(d) (isnan((d - d)) && !isnan(d))
#endif
#ifndef isnan
#define isnan(d) (d != d)
#endif

#ifndef NAN
#ifdef _WIN32
#define NAN sqrt(-1.0)
#else
#define NAN 0.0/0.0
#endif
#endif

typedef struct {
    const unsigned char *json;
    size_t position;
} error;
static error global_error = { NULL, 0 };

CJSON_PUBLIC(const char *) cJSON_GetErrorPtr(void)
{
    return (const char*) (global_error.json + global_error.position);
}

CJSON_PUBLIC(char *) cJSON_GetStringValue(const cJSON * const item)
{
    if (!cJSON_IsString(item))
    {
        return NULL;
    }

    return item->valuestring;
}

CJSON_PUBLIC(double) cJSON_GetNumberValue(const cJSON * const item)
{
    if (!cJSON_IsNumber(item))
    {
        return (double) NAN;
    }

    return item->valuedouble;
}

/* This is a safeguard to prevent copy-pasters from using incompatible C and header files */
#if (CJSON_VERSION_MAJOR != 1) || (CJSON_VERSION_MINOR != 7) || (CJSON_VERSION_PATCH != 16)
    #error cJSON.h and cJSON.c have different versions. Make sure that both have the same.
#endif

CJSON_PUBLIC(const char*) cJSON_Version(void)
{
    static char version[15];
    sprintf(version, "%i.%i.%i", CJSON_VERSION_MAJOR, CJSON_VERSION_MINOR, CJSON_VERSION_PATCH);

    return version;
}

/* Case insensitive string comparison, doesn't consider two NULL pointers equal though */
static int case_insensitive_strcmp(const unsigned char *string1, const unsigned char *string2)
{
    if ((string1 == NULL) || (string2 == NULL))
    {
        return 1;
    }

    if (string1 == string2)
    {
        return 0;
    }

    for(; tolower(*string1) == tolower(*string2); (void)string1++, string2++)
    {
        if (*string1 == '\0')
        {
            return 0;
        }
    }

    return tolower(*string1) - tolower(*string2);
}

typedef struct internal_hooks
{
    void *(CJSON_CDECL *allocate)(size_t size);
    void (CJSON_CDECL *deallocate)(void *pointer);
    void *(CJSON_CDECL *reallocate)(void *pointer, size_t size);
} internal_hooks;

#if defined(_MSC_VER)
/* work around MSVC error C2322: '...' address of dllimport '...' is not static */
static void * CJSON_CDECL internal_malloc(size_t size)
{
    return malloc(size);
}
static void CJSON_CDECL internal_free(void *pointer)
{
    free(pointer);
}
static void * CJSON_CDECL internal_realloc(void *pointer, size_t size)
{
    return realloc(pointer, size);
}
#else
#define internal_malloc malloc
#define internal_free free
#define internal_realloc realloc
#endif

/* strlen of character literals resolved at compile time */
#define static_strlen(string_literal) (sizeof(string_literal) - sizeof(""))

static internal_hooks global_hooks = { internal_malloc, internal_free, internal_realloc };

static unsigned char* cJSON_strdup(const unsigned char* string, const internal_hooks * const hooks)
{
    size_t length = 0;
    unsigned char *copy = NULL;

    if (string == NULL)
    {
        return NULL;
    }

    length = strlen((const char*)string) + sizeof("");
    copy = (unsigned char*)hooks->allocate(length);
    if (copy == NULL)
    {
        return NULL;
    }
    memcpy(copy, string, length);

    return copy;
}

CJSON_PUBLIC(void) cJSON_InitHooks(cJSON_Hooks* hooks)
{
    if (hooks == NULL)
    {
        /* Reset hooks */
        global_hooks.allocate = malloc;
        global_hooks.deallocate = free;
        global_hooks.reallocate = realloc;
        return;
    }

    global_hooks.allocate = malloc;
    if (hooks->malloc_fn != NULL)
    {
        global_hooks.allocate = hooks->malloc_fn;
    }

    global_hooks.deallocate = free;
    if (hooks->free_fn != NULL)
    {
        global_hooks.deallocate = hooks->free_fn;
    }

    /* use realloc only if both free and malloc are used */
    global_hooks.reallocate = NULL;
    if ((global_hooks.allocate == malloc) && (global_hooks.deallocate == free))
    {
        global_hooks.reallocate = realloc;
    }
}

/* Internal constructor. */
static cJSON *cJSON_New_Item(const internal_hooks * const hooks)
{
    cJSON* node = (cJSON*)hooks->allocate(sizeof(cJSON));
    if (node)
    {
        memset(node, '\0', sizeof(cJSON));
    }

    return node;
}

/* Delete a cJSON structure. */
CJSON_PUBLIC(void) cJSON_Delete(cJSON *item)
{
    cJSON *next = NULL;
    while (item != NULL)
    {
        next = item->next;
        if (!(item->type & cJSON_IsReference) && (item->child != NULL))
        {
            cJSON_Delete(item->child);
        }
        if (!(item->type & cJSON_IsReference) && (item->valuestring != NULL))
        {
            global_hooks.deallocate(item->valuestring);
        }
        if (!(item->type & cJSON_StringIsConst) && (item->string != NULL))
        {
            global_hooks.deallocate(item->string);
        }
        global_hooks.deallocate(item);
        item = next;
    }
}

/* get the decimal point character of the current locale */
static unsigned char get_decimal_point(void)
{
#ifdef ENABLE_LOCALES
    struct lconv *lconv = localeconv();
    return (unsigned char) lconv->decimal_point[0];
#else
    return '.';
#endif
}

typedef struct
{
    const unsigned char *content;
    size_t length;
    size_t offset;
    size_t depth; /* How deeply nested (in arrays/objects) is the input at the current offset. */
    internal_hooks hooks;
} parse_buffer;

/* check if the given size is left to read in a given parse buffer (starting with 1) */
#define can_read(buffer, size) ((buffer != NULL) && (((buffer)->offset + size) <= (buffer)->length))
/* check if the buffer can be accessed at the given index (starting with 0) */
#define can_access_at_index(buffer, index) ((buffer != NULL) && (((buffer)->offset + index) < (buffer)->length))
#define cannot_access_at_index(buffer, index) (!can_access_at_index(buffer, index))
/* get a pointer to the buffer at the position */
#define buffer_at_offset(buffer) ((buffer)->content + (buffer)->offset)

/* Parse the input text to generate a number, and populate the result into item. */
static cJSON_bool parse_number(cJSON * const item, parse_buffer * const input_buffer)
{
    double number = 0;
    unsigned char *after_end = NULL;
    unsigned char number_c_string[64];
    unsigned char decimal_point = get_decimal_point();
    size_t i = 0;

    if ((input_buffer == NULL) || (input_buffer->content == NULL))
    {
        return false;
    }

    /* copy the number into a temporary buffer and replace '.' with the decimal point
     * of the current locale (for strtod)
     * This also takes care of '\0' not necessarily being available for marking the end of the input */
    for (i = 0; (i < (sizeof(number_c_string) - 1)) && can_access_at_index(input_buffer, i); i++)
    {
        switch (buffer_at_offset(input_buffer)[i])
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '+':
            case '-':
            case 'e':
            case 'E':
                number_c_string[i] = buffer_at_offset(input_buffer)[i];
                break;

            case '.':
                number_c_string[i] = decimal_point;
                break;

            default:
                goto loop_end;
        }
    }
loop_end:
    number_c_string[i] = '\0';

    number = strtod((const char*)number_c_string, (char**)&after_end);
    if (number_c_string == after_end)
    {
        return false; /* parse_error */
    }

    item->valuedouble = number;

    /* use saturation in case of overflow */
    if (number >= INT_MAX)
    {
        item->valueint = INT_MAX;
    }
    else if (number <= (double)INT_MIN)
    {
        item->valueint = INT_MIN;
    }
    else
    {
        item->valueint = (int)number;
    }

    item->type = cJSON_Number;

    input_buffer->offset += (size_t)(after_end - number_c_string);
    return true;
}

/* don't ask me, but the original cJSON_SetNumberValue returns an integer or double */
CJSON_PUBLIC(double) cJSON_SetNumberHelper(cJSON *object, double number)
{
    if (number >= INT_MAX)
    {
        object->valueint = INT_MAX;
    }
    else if (number <= (double)INT_MIN)
    {
        object->valueint = INT_MIN;
    }
    else
    {
        object->valueint = (int)number;
    }

    return object->valuedouble = number;
}

CJSON_PUBLIC(char*) cJSON_SetValuestring(cJSON *object, const char *valuestring)
{
    char *copy = NULL;
    /* if object's type is not cJSON_String or is cJSON_IsReference, it should not set valuestring */
    if (!(object->type & cJSON_String) || (object->type & cJSON_IsReference))
    {
        return NULL;
    }
    if (strlen(valuestring) <= strlen(object->valuestring))
    {
        strcpy(object->valuestring, valuestring);
        return object->valuestring;
    }
    copy = (char*) cJSON_strdup((const unsigned char*)valuestring, &global_hooks);
    if (copy == NULL)
    {
        return NULL;
    }
    if (object->valuestring != NULL)
    {
        cJSON_free(object->valuestring);
    }
    object->valuestring = copy;

    return copy;
}

typedef struct
{
    unsigned char *buffer;
    size_t length;
    size_t offset;
    size_t depth; /* current nesting depth (for formatted printing) */
    cJSON_bool noalloc;
    cJSON_bool format; /* is this print a formatted print */
    internal_hooks hooks;
} printbuffer;

/* realloc printbuffer if necessary to have at least "needed" bytes more */
static unsigned char* ensure(printbuffer * const p, size_t needed)
{
    unsigned char *newbuffer = NULL;
    size_t newsize = 0;

    if ((p == NULL) || (p->buffer == NULL))
    {
        return NULL;
    }

    if ((p->length > 0) && (p->offset >= p->length))
    {
        /* make sure that offset is valid */
        return NULL;
    }

    if (needed > INT_MAX)
    {
        /* sizes bigger than INT_MAX are currently not supported */
        return NULL;
    }

    needed += p->offset + 1;
    if (needed <= p->length)
    {
        return p->buffer + p->offset;
    }

    if (p->noalloc) {
        return NULL;
    }

    /* calculate new buffer size */
    if (needed > (INT_MAX / 2))
    {
        /* overflow of int, use INT_MAX if possible */
        if (needed <= INT_MAX)
        {
            newsize = INT_MAX;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        newsize = needed * 2;
    }

    if (p->hooks.reallocate != NULL)
    {
        /* reallocate with realloc if available */
        newbuffer = (unsigned char*)p->hooks.reallocate(p->buffer, newsize);
        if (newbuffer == NULL)
        {
            p->hooks.deallocate(p->buffer);
            p->length = 0;
            p->buffer = NULL;

            return NULL;
        }
    }
    else
    {
        /* otherwise reallocate manually */
        newbuffer = (unsigned char*)p->hooks.allocate(newsize);
        if (!newbuffer)
        {
            p->hooks.deallocate(p->buffer);
            p->length = 0;
            p->buffer = NULL;

            return NULL;
        }

        memcpy(newbuffer, p->buffer, p->offset + 1);
        p->hooks.deallocate(p->buffer);
    }
    p->length = newsize;
    p->buffer = newbuffer;

    return newbuffer + p->offset;
}

/* calculate the new length of the string in a printbuffer and update the offset */
static void update_offset(printbuffer * const buffer)
{
    const unsigned char *buffer_pointer = NULL;
    if ((buffer == NULL) || (buffer->buffer == NULL))
    {
        return;
    }
    buffer_pointer = buffer->buffer + buffer->offset;

    buffer->offset += strlen((const char*)buffer_pointer);
}

/* securely comparison of floating-point variables */
static cJSON_bool compare_double(double a, double b)
{
    double maxVal = fabs(a) > fabs(b) ? fabs(a) : fabs(b);
    return (fabs(a - b) <= maxVal * DBL_EPSILON);
}

/* Render the number nicely from the given item into a string. */
static cJSON_bool print_number(const cJSON * const item, printbuffer * const output_buffer)
{
    unsigned char *output_pointer = NULL;
    double d = item->valuedouble;
    int length = 0;
    size_t i = 0;
    unsigned char number_buffer[26] = {0}; /* temporary buffer to print the number into */
    unsigned char decimal_point = get_decimal_point();
    double test = 0.0;

    if (output_buffer == NULL)
    {
        return false;
    }

    /* This checks for NaN and Infinity */
    if (isnan(d) || isinf(d))
    {
        length = sprintf((char*)number_buffer, "null");
    }
	else if(d == (double)item->valueint)
	{
		length = sprintf((char*)number_buffer, "%d", item->valueint);
	}
    else
    {
        /* Try 15 decimal places of precision to avoid nonsignificant nonzero digits */
        length = sprintf((char*)number_buffer, "%1.15g", d);

        /* Check whether the original double can be recovered */
        if ((sscanf((char*)number_buffer, "%lg", &test) != 1) || !compare_double((double)test, d))
        {
            /* If not, print with 17 decimal places of precision */
            length = sprintf((char*)number_buffer, "%1.17g", d);
        }
    }

    /* sprintf failed or buffer overrun occurred */
    if ((length < 0) || (length > (int)(sizeof(number_buffer) - 1)))
    {
        return false;
    }

    /* reserve appropriate space in the output */
    output_pointer = ensure(output_buffer, (size_t)length + sizeof(""));
    if (output_pointer == NULL)
    {
        return false;
    }

    /* copy the printed number to the output and replace locale
     * dependent decimal point with '.' */
    for (i = 0; i < ((size_t)length); i++)
    {
        if (number_buffer[i] == decimal_point)
        {
            output_pointer[i] = '.';
            continue;
        }

        output_pointer[i] = number_buffer[i];
    }
    output_pointer[i] = '\0';

    output_buffer->offset += (size_t)length;

    return true;
}

/* parse 4 digit hexadecimal number */
static unsigned parse_hex4(const unsigned char * const input)
{
    unsigned int h = 0;
    size_t i = 0;

    for (i = 0; i < 4; i++)
    {
        /* parse digit */
        if ((input[i] >= '0') && (input[i] <= '9'))
        {
            h += (unsigned int) input[i] - '0';
        }
        else if ((input[i] >= 'A') && (input[i] <= 'F'))
        {
            h += (unsigned int) 10 + input[i] - 'A';
        }
        else if ((input[i] >= 'a') && (input[i] <= 'f'))
        {
            h += (unsigned int) 10 + input[i] - 'a';
        }
        else /* invalid */
        {
            return 0;
        }

        if (i < 3)
        {
            /* shift left to make place for the next nibble */
            h = h << 4;
        }
    }

    return h;
}

/* converts a UTF-16 literal to UTF-8
 * A literal can be one or two sequences of the form \uXXXX */
static unsigned char utf16_literal_to_utf8(const unsigned char * const input_pointer, const unsigned char * const input_end, unsigned char **output_pointer)
{
    long unsigned int codepoint = 0;
    unsigned int first_code = 0;
    const unsigned char *first_sequence = input_pointer;
    unsigned char utf8_length = 0;
    unsigned char utf8_position = 0;
    unsigned char sequence_length = 0;
    unsigned char first_byte_mark = 0;

    if ((input_end - first_sequence) < 6)
    {
        /* input ends unexpectedly */
        goto fail;
    }

    /* get the first utf16 sequence */
    first_code = parse_hex4(first_sequence + 2);

    /* check that the code is valid */
    if (((first_code >= 0xDC00) && (first_code <= 0xDFFF)))
    {
        goto fail;
    }

    /* UTF16 surrogate pair */
    if ((first_code >= 0xD800) && (first_code <= 0xDBFF))
    {
        const unsigned char *second_sequence = first_sequence + 6;
        unsigned int second_code = 0;
        sequence_length = 12; /* \uXXXX\uXXXX */

        if ((input_end - second_sequence) < 6)
        {
            /* input ends unexpectedly */
            goto fail;
        }

        if ((second_sequence[0] != '\\') || (second_sequence[1] != 'u'))
        {
            /* missing second half of the surrogate pair */
            goto fail;
        }

        /* get the second utf16 sequence */
        second_code = parse_hex4(second_sequence + 2);
        /* check that the code is valid */
        if ((second_code < 0xDC00) || (second_code > 0xDFFF))
        {
            /* invalid second half of the surrogate pair */
            goto fail;
        }


        /* calculate the unicode codepoint from the surrogate pair */
        codepoint = 0x10000 + (((first_code & 0x3FF) << 10) | (second_code & 0x3FF));
    }
    else
    {
        sequence_length = 6; /* \uXXXX */
        codepoint = first_code;
    }

    /* encode as UTF-8
     * takes at maximum 4 bytes to encode:
     * 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
    if (codepoint < 0x80)
    {
        /* normal ascii, encoding 0xxxxxxx */
        utf8_length = 1;
    }
    else if (codepoint < 0x800)
    {
        /* two bytes, encoding 110xxxxx 10xxxxxx */
        utf8_length = 2;
        first_byte_mark = 0xC0; /* 11000000 */
    }
    else if (codepoint < 0x10000)
    {
        /* three bytes, encoding 1110xxxx 10xxxxxx 10xxxxxx */
        utf8_length = 3;
        first_byte_mark = 0xE0; /* 11100000 */
    }
    else if (codepoint <= 0x10FFFF)
    {
        /* four bytes, encoding 1110xxxx 10xxxxxx 10xxxxxx 10xxxxxx */
        utf8_length = 4;
        first_byte_mark = 0xF0; /* 11110000 */
    }
    else
    {
        /* invalid unicode codepoint */
        goto fail;
    }

    /* encode as utf8 */
    for (utf8_position = (unsigned char)(utf8_length - 1); utf8_position > 0; utf8_position--)
    {
        /* 10xxxxxx */
        (*output_pointer)[utf8_position] = (unsigned char)((codepoint | 0x80) & 0xBF);
        codepoint >>= 6;
    }
    /* encode first byte */
    if (utf8_length > 1)
    {
        (*output_pointer)[0] = (unsigned char)((codepoint | first_byte_mark) & 0xFF);
    }
    else
    {
        (*output_pointer)[0] = (unsigned char)(codepoint & 0x7F);
    }

    *output_pointer += utf8_length;

    return sequence_length;

fail:
    return 0;
}

/* Parse the input text into an unescaped cinput, and populate item. */
static cJSON_bool parse_string(cJSON * const item, parse_buffer * const input_buffer)
{
    const unsigned char *input_pointer = buffer_at_offset(input_buffer) + 1;
    const unsigned char *input_end = buffer_at_offset(input_buffer) + 1;
    unsigned char *output_pointer = NULL;
    unsigned char *output = NULL;

    /* not a string */
    if (buffer_at_offset(input_buffer)[0] != '\"')
    {
        goto fail;
    }

    {
        /* calculate approximate size of the output (overestimate) */
        size_t allocation_length = 0;
        size_t skipped_bytes = 0;
        while (((size_t)(input_end - input_buffer->content) < input_buffer->length) && (*input_end != '\"'))
        {
            /* is escape sequence */
            if (input_end[0] == '\\')
            {
                if ((size_t)(input_end + 1 - input_buffer->content) >= input_buffer->length)
                {
                    /* prevent buffer overflow when last input character is a backslash */
                    goto fail;
                }
                skipped_bytes++;
                input_end++;
            }
            input_end++;
        }
        if (((size_t)(input_end - input_buffer->content) >= input_buffer->length) || (*input_end != '\"'))
        {
            goto fail; /* string ended unexpectedly */
        }

        /* This is at most how much we need for the output */
        allocation_length = (size_t) (input_end - buffer_at_offset(input_buffer)) - skipped_bytes;
        output = (unsigned char*)input_buffer->hooks.allocate(allocation_length + sizeof(""));
        if (output == NULL)
        {
            goto fail; /* allocation failure */
        }
    }

    output_pointer = output;
    /* loop through the string literal */
    while (input_pointer < input_end)
    {
        if (*input_pointer != '\\')
        {
            *output_pointer++ = *input_pointer++;
        }
        /* escape sequence */
        else
        {
            unsigned char sequence_length = 2;
            if ((input_end - input_pointer) < 1)
            {
                goto fail;
            }

            switch (input_pointer[1])
            {
                case 'b':
                    *output_pointer++ = '\b';
                    break;
                case 'f':
                    *output_pointer++ = '\f';
                    break;
                case 'n':
                    *output_pointer++ = '\n';
                    break;
                case 'r':
                    *output_pointer++ = '\r';
                    break;
                case 't':
                    *output_pointer++ = '\t';
                    break;
                case '\"':
                case '\\':
                case '/':
                    *output_pointer++ = input_pointer[1];
                    break;

                /* UTF-16 literal */
                case 'u':
                    sequence_length = utf16_literal_to_utf8(input_pointer, input_end, &output_pointer);
                    if (sequence_length == 0)
                    {
                        /* failed to convert UTF16-literal to UTF-8 */
                        goto fail;
                    }
                    break;

                default:
                    goto fail;
            }
            input_pointer += sequence_length;
        }
    }

    /* zero terminate the output */
    *output_pointer = '\0';

    item->type = cJSON_String;
    item->valuestring = (char*)output;

    input_buffer->offset = (size_t) (input_end - input_buffer->content);
    input_buffer->offset++;

    return true;

fail:
    if (output != NULL)
    {
        input_buffer->hooks.deallocate(output);
    }

    if (input_pointer != NULL)
    {
        input_buffer->offset = (size_t)(input_pointer - input_buffer->content);
    }

    return false;
}

/* Render the cstring provided to an escaped version that can be printed. */
static cJSON_bool print_string_ptr(const unsigned char * const input, printbuffer * const output_buffer)
{
    const unsigned char *input_pointer = NULL;
    unsigned char *output = NULL;
    unsigned char *output_pointer = NULL;
    size_t output_length = 0;
    /* numbers of additional characters needed for escaping */
    size_t escape_characters = 0;

    if (output_buffer == NULL)
    {
        return false;
    }

    /* empty string */
    if (input == NULL)
    {
        output = ensure(output_buffer, sizeof("\"\""));
        if (output == NULL)
        {
            return false;
        }
        strcpy((char*)output, "\"\"");

        return true;
    }

    /* set "flag" to 1 if something needs to be escaped */
    for (input_pointer = input; *input_pointer; input_pointer++)
    {
        switch (*input_pointer)
        {
            case '\"':
            case '\\':
            case '\b':
            case '\f':
            case '\n':
            case '\r':
            case '\t':
                /* one character escape sequence */
                escape_characters++;
                break;
            default:
                if (*input_pointer < 32)
                {
                    /* UTF-16 escape sequence uXXXX */
                    escape_characters += 5;
                }
                break;
        }
    }
    output_length = (size_t)(input_pointer - input) + escape_characters;

    output = ensure(output_buffer, output_length + sizeof("\"\""));
    if (output == NULL)
    {
        return false;
    }

    /* no characters have to be escaped */
    if (escape_characters == 0)
    {
        output[0] = '\"';
        memcpy(output + 1, input, output_length);
        output[output_length + 1] = '\"';
        output[output_length + 2] = '\0';

        return true;
    }

    output[0] = '\"';
    output_pointer = output + 1;
    /* copy the string */
    for (input_pointer = input; *input_pointer != '\0'; (void)input_pointer++, output_pointer++)
    {
        if ((*input_pointer > 31) && (*input_pointer != '\"') && (*input_pointer != '\\'))
        {
            /* normal character, copy */
            *output_pointer = *input_pointer;
        }
        else
        {
            /* character needs to be escaped */
            *output_pointer++ = '\\';
            switch (*input_pointer)
            {
                case '\\':
                    *output_pointer = '\\';
                    break;
                case '\"':
                    *output_pointer = '\"';
                    break;
                case '\b':
                    *output_pointer = 'b';
                    break;
                case '\f':
                    *output_pointer = 'f';
                    break;
                case '\n':
                    *output_pointer = 'n';
                    break;
                case '\r':
                    *output_pointer = 'r';
                    break;
                case '\t':
                    *output_pointer = 't';
                    break;
                default:
                    /* escape and print as unicode codepoint */
                    sprintf((char*)output_pointer, "u%04x", *input_pointer);
                    output_pointer += 4;
                    break;
            }
        }
    }
    output[output_length + 1] = '\"';
    output[output_length + 2] = '\0';

    return true;
}

/* Invoke print_string_ptr (which is useful) on an item. */
static cJSON_bool print_string(const cJSON * const item, printbuffer * const p)
{
    return print_string_ptr((unsigned char*)item->valuestring, p);
}

/* Predeclare these prototypes. */
static cJSON_bool parse_value(cJSON * const item, parse_buffer * const input_buffer);
static cJSON_bool print_value(const cJSON * const item, printbuffer * const output_buffer);
static cJSON_bool parse_array(cJSON * const item, parse_buffer * const input_buffer);
static cJSON_bool print_array(const cJSON * const item, printbuffer * const output_buffer);
static cJSON_bool parse_object(cJSON * const item, parse_buffer * const input_buffer);
static cJSON_bool print_object(const cJSON * const item, printbuffer * const output_buffer);

/* Utility to jump whitespace and cr/lf */
static parse_buffer *buffer_skip_whitespace(parse_buffer * const buffer)
{
    if ((buffer == NULL) || (buffer->content == NULL))
    {
        return NULL;
    }

    if (cannot_access_at_index(buffer, 0))
    {
        return buffer;
    }

    while (can_access_at_index(buffer, 0) && (buffer_at_offset(buffer)[0] <= 32))
    {
       buffer->offset++;
    }

    if (buffer->offset == buffer->length)
    {
        buffer->offset--;
    }

    return buffer;
}

/* skip the UTF-8 BOM (byte order mark) if it is at the beginning of a buffer */
static parse_buffer *skip_utf8_bom(parse_buffer * const buffer)
{
    if ((buffer == NULL) || (buffer->content == NULL) || (buffer->offset != 0))
    {
        return NULL;
    }

    if (can_access_at_index(buffer, 4) && (strncmp((const char*)buffer_at_offset(buffer), "\xEF\xBB\xBF", 3) == 0))
    {
        buffer->offset += 3;
    }

    return buffer;
}

CJSON_PUBLIC(cJSON *) cJSON_ParseWithOpts(const char *value, const char **return_parse_end, cJSON_bool require_null_terminated)
{
    size_t buffer_length;

    if (NULL == value)
    {
        return NULL;
    }

    /* Adding null character size due to require_null_terminated. */
    buffer_length = strlen(value) + sizeof("");

    return cJSON_ParseWithLengthOpts(value, buffer_length, return_parse_end, require_null_terminated);
}

/* Parse an object - create a new root, and populate. */
CJSON_PUBLIC(cJSON *) cJSON_ParseWithLengthOpts(const char *value, size_t buffer_length, const char **return_parse_end, cJSON_bool require_null_terminated)
{
    parse_buffer buffer = { 0, 0, 0, 0, { 0, 0, 0 } };
    cJSON *item = NULL;

    /* reset error position */
    global_error.json = NULL;
    global_error.position = 0;

    if (value == NULL || 0 == buffer_length)
    {
        goto fail;
    }

    buffer.content = (const unsigned char*)value;
    buffer.length = buffer_length;
    buffer.offset = 0;
    buffer.hooks = global_hooks;

    item = cJSON_New_Item(&global_hooks);
    if (item == NULL) /* memory fail */
    {
        goto fail;
    }

    if (!parse_value(item, buffer_skip_whitespace(skip_utf8_bom(&buffer))))
    {
        /* parse failure. ep is set. */
        goto fail;
    }

    /* if we require null-terminated JSON without appended garbage, skip and then check for a null terminator */
    if (require_null_terminated)
    {
        buffer_skip_whitespace(&buffer);
        if ((buffer.offset >= buffer.length) || buffer_at_offset(&buffer)[0] != '\0')
        {
            goto fail;
        }
    }
    if (return_parse_end)
    {
        *return_parse_end = (const char*)buffer_at_offset(&buffer);
    }

    return item;

fail:
    if (item != NULL)
    {
        cJSON_Delete(item);
    }

    if (value != NULL)
    {
        error local_error;
        local_error.json = (const unsigned char*)value;
        local_error.position = 0;

        if (buffer.offset < buffer.length)
        {
            local_error.position = buffer.offset;
        }
        else if (buffer.length > 0)
        {
            local_error.position = buffer.length - 1;
        }

        if (return_parse_end != NULL)
        {
            *return_parse_end = (const char*)local_error.json + local_error.position;
        }

        global_error = local_error;
    }

    return NULL;
}

/* Default options for cJSON_Parse */
CJSON_PUBLIC(cJSON *) cJSON_Parse(const char *value)
{
    return cJSON_ParseWithOpts(value, 0, 0);
}

CJSON_PUBLIC(cJSON *) cJSON_ParseWithLength(const char *value, size_t buffer_length)
{
    return cJSON_ParseWithLengthOpts(value, buffer_length, 0, 0);
}

#define cjson_min(a, b) (((a) < (b)) ? (a) : (b))

static unsigned char *print(const cJSON * const item, cJSON_bool format, const internal_hooks * const hooks)
{
    static const size_t default_buffer_size = 256;
    printbuffer buffer[1];
    unsigned char *printed = NULL;

    memset(buffer, 0, sizeof(buffer));

    /* create buffer */
    buffer->buffer = (unsigned char*) hooks->allocate(default_buffer_size);
    buffer->length = default_buffer_size;
    buffer->format = format;
    buffer->hooks = *hooks;
    if (buffer->buffer == NULL)
    {
        goto fail;
    }

    /* print the value */
    if (!print_value(item, buffer))
    {
        goto fail;
    }
    update_offset(buffer);

    /* check if reallocate is available */
    if (hooks->reallocate != NULL)
    {
        printed = (unsigned char*) hooks->reallocate(buffer->buffer, buffer->offset + 1);
        if (printed == NULL) {
            goto fail;
        }
        buffer->buffer = NULL;
    }
    else /* otherwise copy the JSON over to a new buffer */
    {
        printed = (unsigned char*) hooks->allocate(buffer->offset + 1);
        if (printed == NULL)
        {
            goto fail;
        }
        memcpy(printed, buffer->buffer, cjson_min(buffer->length, buffer->offset + 1));
        printed[buffer->offset] = '\0'; /* just to be sure */

        /* free the buffer */
        hooks->deallocate(buffer->buffer);
    }

    return printed;

fail:
    if (buffer->buffer != NULL)
    {
        hooks->deallocate(buffer->buffer);
    }

    if (printed != NULL)
    {
        hooks->deallocate(printed);
    }

    return NULL;
}

/* Render a cJSON item/entity/structure to text. */
CJSON_PUBLIC(char *) cJSON_Print(const cJSON *item)
{
    return (char*)print(item, true, &global_hooks);
}

CJSON_PUBLIC(char *) cJSON_PrintUnformatted(const cJSON *item)
{
    return (char*)print(item, false, &global_hooks);
}

CJSON_PUBLIC(char *) cJSON_PrintBuffered(const cJSON *item, int prebuffer, cJSON_bool fmt)
{
    printbuffer p = { 0, 0, 0, 0, 0, 0, { 0, 0, 0 } };

    if (prebuffer < 0)
    {
        return NULL;
    }

    p.buffer = (unsigned char*)global_hooks.allocate((size_t)prebuffer);
    if (!p.buffer)
    {
        return NULL;
    }

    p.length = (size_t)prebuffer;
    p.offset = 0;
    p.noalloc = false;
    p.format = fmt;
    p.hooks = global_hooks;

    if (!print_value(item, &p))
    {
        global_hooks.deallocate(p.buffer);
        return NULL;
    }

    return (char*)p.buffer;
}

CJSON_PUBLIC(cJSON_bool) cJSON_PrintPreallocated(cJSON *item, char *buffer, const int length, const cJSON_bool format)
{
    printbuffer p = { 0, 0, 0, 0, 0, 0, { 0, 0, 0 } };

    if ((length < 0) || (buffer == NULL))
    {
        return false;
    }

    p.buffer = (unsigned char*)buffer;
    p.length = (size_t)length;
    p.offset = 0;
    p.noalloc = true;
    p.format = format;
    p.hooks = global_hooks;

    return print_value(item, &p);
}

/* Parser core - when encountering text, process appropriately. */
static cJSON_bool parse_value(cJSON * const item, parse_buffer * const input_buffer)
{
    if ((input_buffer == NULL) || (input_buffer->content == NULL))
    {
        return false; /* no input */
    }

    /* parse the different types of values */
    /* null */
    if (can_read(input_buffer, 4) && (strncmp((const char*)buffer_at_offset(input_buffer), "null", 4) == 0))
    {
        item->type = cJSON_NULL;
        input_buffer->offset += 4;
        return true;
    }
    /* false */
    if (can_read(input_buffer, 5) && (strncmp((const char*)buffer_at_offset(input_buffer), "false", 5) == 0))
    {
        item->type = cJSON_False;
        input_buffer->offset += 5;
        return true;
    }
    /* true */
    if (can_read(input_buffer, 4) && (strncmp((const char*)buffer_at_offset(input_buffer), "true", 4) == 0))
    {
        item->type = cJSON_True;
        item->valueint = 1;
        input_buffer->offset += 4;
        return true;
    }
    /* string */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '\"'))
    {
        return parse_string(item, input_buffer);
    }
    /* number */
    if (can_access_at_index(input_buffer, 0) && ((buffer_at_offset(input_buffer)[0] == '-') || ((buffer_at_offset(input_buffer)[0] >= '0') && (buffer_at_offset(input_buffer)[0] <= '9'))))
    {
        return parse_number(item, input_buffer);
    }
    /* array */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '['))
    {
        return parse_array(item, input_buffer);
    }
    /* object */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '{'))
    {
        return parse_object(item, input_buffer);
    }

    return false;
}

/* Render a value to text. */
static cJSON_bool print_value(const cJSON * const item, printbuffer * const output_buffer)
{
    unsigned char *output = NULL;

    if ((item == NULL) || (output_buffer == NULL))
    {
        return false;
    }

    switch ((item->type) & 0xFF)
    {
        case cJSON_NULL:
            output = ensure(output_buffer, 5);
            if (output == NULL)
            {
                return false;
            }
            strcpy((char*)output, "null");
            return true;

        case cJSON_False:
            output = ensure(output_buffer, 6);
            if (output == NULL)
            {
                return false;
            }
            strcpy((char*)output, "false");
            return true;

        case cJSON_True:
            output = ensure(output_buffer, 5);
            if (output == NULL)
            {
                return false;
            }
            strcpy((char*)output, "true");
            return true;

        case cJSON_Number:
            return print_number(item, output_buffer);

        case cJSON_Raw:
        {
            size_t raw_length = 0;
            if (item->valuestring == NULL)
            {
                return false;
            }

            raw_length = strlen(item->valuestring) + sizeof("");
            output = ensure(output_buffer, raw_length);
            if (output == NULL)
            {
                return false;
            }
            memcpy(output, item->valuestring, raw_length);
            return true;
        }

        case cJSON_String:
            return print_string(item, output_buffer);

        case cJSON_Array:
            return print_array(item, output_buffer);

        case cJSON_Object:
            return print_object(item, output_buffer);

        default:
            return false;
    }
}

/* Build an array from input text. */
static cJSON_bool parse_array(cJSON * const item, parse_buffer * const input_buffer)
{
    cJSON *head = NULL; /* head of the linked list */
    cJSON *current_item = NULL;

    if (input_buffer->depth >= CJSON_NESTING_LIMIT)
    {
        return false; /* to deeply nested */
    }
    input_buffer->depth++;

    if (buffer_at_offset(input_buffer)[0] != '[')
    {
        /* not an array */
        goto fail;
    }

    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ']'))
    {
        /* empty array */
        goto success;
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(input_buffer, 0))
    {
        input_buffer->offset--;
        goto fail;
    }

    /* step back to character in front of the first element */
    input_buffer->offset--;
    /* loop through the comma separated array elements */
    do
    {
        /* allocate next item */
        cJSON *new_item = cJSON_New_Item(&(input_buffer->hooks));
        if (new_item == NULL)
        {
            goto fail; /* allocation failure */
        }

        /* attach next item to list */
        if (head == NULL)
        {
            /* start the linked list */
            current_item = head = new_item;
        }
        else
        {
            /* add to the end and advance */
            current_item->next = new_item;
            new_item->prev = current_item;
            current_item = new_item;
        }

        /* parse next value */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_value(current_item, input_buffer))
        {
            goto fail; /* failed to parse value */
        }
        buffer_skip_whitespace(input_buffer);
    }
    while (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ','));

    if (cannot_access_at_index(input_buffer, 0) || buffer_at_offset(input_buffer)[0] != ']')
    {
        goto fail; /* expected end of array */
    }

success:
    input_buffer->depth--;

    if (head != NULL) {
        head->prev = current_item;
    }

    item->type = cJSON_Array;
    item->child = head;

    input_buffer->offset++;

    return true;

fail:
    if (head != NULL)
    {
        cJSON_Delete(head);
    }

    return false;
}

/* Render an array to text */
static cJSON_bool print_array(const cJSON * const item, printbuffer * const output_buffer)
{
    unsigned char *output_pointer = NULL;
    size_t length = 0;
    cJSON *current_element = item->child;

    if (output_buffer == NULL)
    {
        return false;
    }

    /* Compose the output array. */
    /* opening square bracket */
    output_pointer = ensure(output_buffer, 1);
    if (output_pointer == NULL)
    {
        return false;
    }

    *output_pointer = '[';
    output_buffer->offset++;
    output_buffer->depth++;

    while (current_element != NULL)
    {
        if (!print_value(current_element, output_buffer))
        {
            return false;
        }
        update_offset(output_buffer);
        if (current_element->next)
        {
            length = (size_t) (output_buffer->format ? 2 : 1);
            output_pointer = ensure(output_buffer, length + 1);
            if (output_pointer == NULL)
            {
                return false;
            }
            *output_pointer++ = ',';
            if(output_buffer->format)
            {
                *output_pointer++ = ' ';
            }
            *output_pointer = '\0';
            output_buffer->offset += length;
        }
        current_element = current_element->next;
    }

    output_pointer = ensure(output_buffer, 2);
    if (output_pointer == NULL)
    {
        return false;
    }
    *output_pointer++ = ']';
    *output_pointer = '\0';
    output_buffer->depth--;

    return true;
}

/* Build an object from the text. */
static cJSON_bool parse_object(cJSON * const item, parse_buffer * const input_buffer)
{
    cJSON *head = NULL; /* linked list head */
    cJSON *current_item = NULL;

    if (input_buffer->depth >= CJSON_NESTING_LIMIT)
    {
        return false; /* to deeply nested */
    }
    input_buffer->depth++;

    if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != '{'))
    {
        goto fail; /* not an object */
    }

    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '}'))
    {
        goto success; /* empty object */
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(input_buffer, 0))
    {
        input_buffer->offset--;
        goto fail;
    }

    /* step back to character in front of the first element */
    input_buffer->offset--;
    /* loop through the comma separated array elements */
    do
    {
        /* allocate next item */
        cJSON *new_item = cJSON_New_Item(&(input_buffer->hooks));
        if (new_item == NULL)
        {
            goto fail; /* allocation failure */
        }

        /* attach next item to list */
        if (head == NULL)
        {
            /* start the linked list */
            current_item = head = new_item;
        }
        else
        {
            /* add to the end and advance */
            current_item->next = new_item;
            new_item->prev = current_item;
            current_item = new_item;
        }

        /* parse the name of the child */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_string(current_item, input_buffer))
        {
            goto fail; /* failed to parse name */
        }
        buffer_skip_whitespace(input_buffer);

        /* swap valuestring and string, because we parsed the name */
        current_item->string = current_item->valuestring;
        current_item->valuestring = NULL;

        if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != ':'))
        {
            goto fail; /* invalid object */
        }

        /* parse the value */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_value(current_item, input_buffer))
        {
            goto fail; /* failed to parse value */
        }
        buffer_skip_whitespace(input_buffer);
    }
    while (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ','));

    if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != '}'))
    {
        goto fail; /* expected end of object */
    }

success:
    input_buffer->depth--;

    if (head != NULL) {
        head->prev = current_item;
    }

    item->type = cJSON_Object;
    item->child = head;

    input_buffer->offset++;
    return true;

fail:
    if (head != NULL)
    {
        cJSON_Delete(head);
    }

    return false;
}

/* Render an object to text. */
static cJSON_bool print_object(const cJSON * const item, printbuffer * const output_buffer)
{
    unsigned char *output_pointer = NULL;
    size_t length = 0;
    cJSON *current_item = item->child;

    if (output_buffer == NULL)
    {
        return false;
    }

    /* Compose the output: */
    length = (size_t) (output_buffer->format ? 2 : 1); /* fmt: {\n */
    output_pointer = ensure(output_buffer, length + 1);
    if (output_pointer == NULL)
    {
        return false;
    }

    *output_pointer++ = '{';
    output_buffer->depth++;
    if (output_buffer->format)
    {
        *output_pointer++ = '\n';
    }
    output_buffer->offset += length;

    while (current_item)
    {
        if (output_buffer->format)
        {
            size_t i;
            output_pointer = ensure(output_buffer, output_buffer->depth);
            if (output_pointer == NULL)
            {
                return false;
            }
            for (i = 0; i < output_buffer->depth; i++)
            {
                *output_pointer++ = '\t';
            }
            output_buffer->offset += output_buffer->depth;
        }

        /* print key */
        if (!print_string_ptr((unsigned char*)current_item->string, output_buffer))
        {
            return false;
        }
        update_offset(output_buffer);

        length = (size_t) (output_buffer->format ? 2 : 1);
        output_pointer = ensure(output_buffer, length);
        if (output_pointer == NULL)
        {
            return false;
        }
        *output_pointer++ = ':';
        if (output_buffer->format)
        {
            *output_pointer++ = '\t';
        }
        output_buffer->offset += length;

        /* print value */
        if (!print_value(current_item, output_buffer))
        {
            return false;
        }
        update_offset(output_buffer);

        /* print comma if not last */
        length = ((size_t)(output_buffer->format ? 1 : 0) + (size_t)(current_item->next ? 1 : 0));
        output_pointer = ensure(output_buffer, length + 1);
        if (output_pointer == NULL)
        {
            return false;
        }
        if (current_item->next)
        {
            *output_pointer++ = ',';
        }

        if (output_buffer->format)
        {
            *output_pointer++ = '\n';
        }
        *output_pointer = '\0';
        output_buffer->offset += length;

        current_item = current_item->next;
    }

    output_pointer = ensure(output_buffer, output_buffer->format ? (output_buffer->depth + 1) : 2);
    if (output_pointer == NULL)
    {
        return false;
    }
    if (output_buffer->format)
    {
        size_t i;
        for (i = 0; i < (output_buffer->depth - 1); i++)
        {
            *output_pointer++ = '\t';
        }
    }
    *output_pointer++ = '}';
    *output_pointer = '\0';
    output_buffer->depth--;

    return true;
}

/* Get Array size/item / object item. */
CJSON_PUBLIC(int) cJSON_GetArraySize(const cJSON *array)
{
    cJSON *child = NULL;
    size_t size = 0;

    if (array == NULL)
    {
        return 0;
    }

    child = array->child;

    while(child != NULL)
    {
        size++;
        child = child->next;
    }

    /* FIXME: Can overflow here. Cannot be fixed without breaking the API */

    return (int)size;
}

static cJSON* get_array_item(const cJSON *array, size_t index)
{
    cJSON *current_child = NULL;

    if (array == NULL)
    {
        return NULL;
    }

    current_child = array->child;
    while ((current_child != NULL) && (index > 0))
    {
        index--;
        current_child = current_child->next;
    }

    return current_child;
}

CJSON_PUBLIC(cJSON *) cJSON_GetArrayItem(const cJSON *array, int index)
{
    if (index < 0)
    {
        return NULL;
    }

    return get_array_item(array, (size_t)index);
}

static cJSON *get_object_item(const cJSON * const object, const char * const name, const cJSON_bool case_sensitive)
{
    cJSON *current_element = NULL;

    if ((object == NULL) || (name == NULL))
    {
        return NULL;
    }

    current_element = object->child;
    if (case_sensitive)
    {
        while ((current_element != NULL) && (current_element->string != NULL) && (strcmp(name, current_element->string) != 0))
        {
            current_element = current_element->next;
        }
    }
    else
    {
        while ((current_element != NULL) && (case_insensitive_strcmp((const unsigned char*)name, (const unsigned char*)(current_element->string)) != 0))
        {
            current_element = current_element->next;
        }
    }

    if ((current_element == NULL) || (current_element->string == NULL)) {
        return NULL;
    }

    return current_element;
}

CJSON_PUBLIC(cJSON *) cJSON_GetObjectItem(const cJSON * const object, const char * const string)
{
    return get_object_item(object, string, false);
}

CJSON_PUBLIC(cJSON *) cJSON_GetObjectItemCaseSensitive(const cJSON * const object, const char * const string)
{
    return get_object_item(object, string, true);
}

CJSON_PUBLIC(cJSON_bool) cJSON_HasObjectItem(const cJSON *object, const char *string)
{
    return cJSON_GetObjectItem(object, string) ? 1 : 0;
}

/* Utility for array list handling. */
static void suffix_object(cJSON *prev, cJSON *item)
{
    prev->next = item;
    item->prev = prev;
}

/* Utility for handling references. */
static cJSON *create_reference(const cJSON *item, const internal_hooks * const hooks)
{
    cJSON *reference = NULL;
    if (item == NULL)
    {
        return NULL;
    }

    reference = cJSON_New_Item(hooks);
    if (reference == NULL)
    {
        return NULL;
    }

    memcpy(reference, item, sizeof(cJSON));
    reference->string = NULL;
    reference->type |= cJSON_IsReference;
    reference->next = reference->prev = NULL;
    return reference;
}

static cJSON_bool add_item_to_array(cJSON *array, cJSON *item)
{
    cJSON *child = NULL;

    if ((item == NULL) || (array == NULL) || (array == item))
    {
        return false;
    }

    child = array->child;
    /*
     * To find the last item in array quickly, we use prev in array
     */
    if (child == NULL)
    {
        /* list is empty, start new one */
        array->child = item;
        item->prev = item;
        item->next = NULL;
    }
    else
    {
        /* append to the end */
        if (child->prev)
        {
            suffix_object(child->prev, item);
            array->child->prev = item;
        }
    }

    return true;
}

/* Add item to array/object. */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToArray(cJSON *array, cJSON *item)
{
    return add_item_to_array(array, item);
}

#if defined(__clang__) || (defined(__GNUC__)  && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 5))))
    #pragma GCC diagnostic push
#endif
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
/* helper function to cast away const */
static void* cast_away_const(const void* string)
{
    return (void*)string;
}
#if defined(__clang__) || (defined(__GNUC__)  && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 5))))
    #pragma GCC diagnostic pop
#endif


static cJSON_bool add_item_to_object(cJSON * const object, const char * const string, cJSON * const item, const internal_hooks * const hooks, const cJSON_bool constant_key)
{
    char *new_key = NULL;
    int new_type = cJSON_Invalid;

    if ((object == NULL) || (string == NULL) || (item == NULL) || (object == item))
    {
        return false;
    }

    if (constant_key)
    {
        new_key = (char*)cast_away_const(string);
        new_type = item->type | cJSON_StringIsConst;
    }
    else
    {
        new_key = (char*)cJSON_strdup((const unsigned char*)string, hooks);
        if (new_key == NULL)
        {
            return false;
        }

        new_type = item->type & ~cJSON_StringIsConst;
    }

    if (!(item->type & cJSON_StringIsConst) && (item->string != NULL))
    {
        hooks->deallocate(item->string);
    }

    item->string = new_key;
    item->type = new_type;

    return add_item_to_array(object, item);
}

CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item)
{
    return add_item_to_object(object, string, item, &global_hooks, false);
}

/* Add an item to an object with constant string as key */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToObjectCS(cJSON *object, const char *string, cJSON *item)
{
    return add_item_to_object(object, string, item, &global_hooks, true);
}

CJSON_PUBLIC(cJSON_bool) cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item)
{
    if (array == NULL)
    {
        return false;
    }

    return add_item_to_array(array, create_reference(item, &global_hooks));
}

CJSON_PUBLIC(cJSON_bool) cJSON_AddItemReferenceToObject(cJSON *object, const char *string, cJSON *item)
{
    if ((object == NULL) || (string == NULL))
    {
        return false;
    }

    return add_item_to_object(object, string, create_reference(item, &global_hooks), &global_hooks, false);
}

CJSON_PUBLIC(cJSON*) cJSON_AddNullToObject(cJSON * const object, const char * const name)
{
    cJSON *null = cJSON_CreateNull();
    if (add_item_to_object(object, name, null, &global_hooks, false))
    {
        return null;
    }

    cJSON_Delete(null);
    return NULL;
}

CJSON_PUBLIC(cJSON*) cJSON_AddTrueToObject(cJSON * const object, const char * const name)
{
    cJSON *true_item = cJSON_CreateTrue();
    if (add_item_to_object(object, name, true_item, &global_hooks, false))
    {
        return true_item;
    }

    cJSON_Delete(true_item);
    return NULL;
}

CJSON_PUBLIC(cJSON*) cJSON_AddFalseToObject(cJSON * const object, const char * const name)
{
    cJSON *false_item = cJSON_CreateFalse();
    if (add_item_to_object(object, name, false_item, &global_hooks, false))
    {
        return false_item;
    }

    cJSON_Delete(false_item);
    return NULL;
}

CJSON_PUBLIC(cJSON*) cJSON_AddBoolToObject(cJSON * const object, const char * const name, const cJSON_bool boolean)
{
    cJSON *bool_item = cJSON_CreateBool(boolean);
    if (add_item_to_object(object, name, bool_item, &global_hooks, false))
    {
        return bool_item;
    }

    cJSON_Delete(bool_item);
    return NULL;
}

CJSON_PUBLIC(cJSON*) cJSON_AddNumberToObject(cJSON * const object, const char * const name, const double number)
{
    cJSON *number_item = cJSON_CreateNumber(number);
    if (add_item_to_object(object, name, number_item, &global_hooks, false))
    {
        return number_item;
    }

    cJSON_Delete(number_item);
    return NULL;
}

CJSON_PUBLIC(cJSON*) cJSON_AddStringToObject(cJSON * const object, const char * const name, const char * const string)
{
    cJSON *string_item = cJSON_CreateString(string);
    if (add_item_to_object(object, name, string_item, &global_hooks, false))
    {
        return string_item;
    }

    cJSON_Delete(string_item);
    return NULL;
}

CJSON_PUBLIC(cJSON*) cJSON_AddRawToObject(cJSON * const object, const char * const name, const char * const raw)
{
    cJSON *raw_item = cJSON_CreateRaw(raw);
    if (add_item_to_object(object, name, raw_item, &global_hooks, false))
    {
        return raw_item;
    }

    cJSON_Delete(raw_item);
    return NULL;
}

CJSON_PUBLIC(cJSON*) cJSON_AddObjectToObject(cJSON * const object, const char * const name)
{
    cJSON *object_item = cJSON_CreateObject();
    if (add_item_to_object(object, name, object_item, &global_hooks, false))
    {
        return object_item;
    }

    cJSON_Delete(object_item);
    return NULL;
}

CJSON_PUBLIC(cJSON*) cJSON_AddArrayToObject(cJSON * const object, const char * const name)
{
    cJSON *array = cJSON_CreateArray();
    if (add_item_to_object(object, name, array, &global_hooks, false))
    {
        return array;
    }

    cJSON_Delete(array);
    return NULL;
}

CJSON_PUBLIC(cJSON *) cJSON_DetachItemViaPointer(cJSON *parent, cJSON * const item)
{
    if ((parent == NULL) || (item == NULL))
    {
        return NULL;
    }

    if (item != parent->child)
    {
        /* not the first element */
        item->prev->next = item->next;
    }
    if (item->next != NULL)
    {
        /* not the last element */
        item->next->prev = item->prev;
    }

    if (item == parent->child)
    {
        /* first element */
        parent->child = item->next;
    }
    else if (item->next == NULL)
    {
        /* last element */
        parent->child->prev = item->prev;
    }

    /* make sure the detached item doesn't point anywhere anymore */
    item->prev = NULL;
    item->next = NULL;

    return item;
}

CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromArray(cJSON *array, int which)
{
    if (which < 0)
    {
        return NULL;
    }

    return cJSON_DetachItemViaPointer(array, get_array_item(array, (size_t)which));
}

CJSON_PUBLIC(void) cJSON_DeleteItemFromArray(cJSON *array, int which)
{
    cJSON_Delete(cJSON_DetachItemFromArray(array, which));
}

CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromObject(cJSON *object, const char *string)
{
    cJSON *to_detach = cJSON_GetObjectItem(object, string);

    return cJSON_DetachItemViaPointer(object, to_detach);
}

CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromObjectCaseSensitive(cJSON *object, const char *string)
{
    cJSON *to_detach = cJSON_GetObjectItemCaseSensitive(object, string);

    return cJSON_DetachItemViaPointer(object, to_detach);
}

CJSON_PUBLIC(void) cJSON_DeleteItemFromObject(cJSON *object, const char *string)
{
    cJSON_Delete(cJSON_DetachItemFromObject(object, string));
}

CJSON_PUBLIC(void) cJSON_DeleteItemFromObjectCaseSensitive(cJSON *object, const char *string)
{
    cJSON_Delete(cJSON_DetachItemFromObjectCaseSensitive(object, string));
}

/* Replace array/object items with new ones. */
CJSON_PUBLIC(cJSON_bool) cJSON_InsertItemInArray(cJSON *array, int which, cJSON *newitem)
{
    cJSON *after_inserted = NULL;

    if (which < 0)
    {
        return false;
    }

    after_inserted = get_array_item(array, (size_t)which);
    if (after_inserted == NULL)
    {
        return add_item_to_array(array, newitem);
    }

    newitem->next = after_inserted;
    newitem->prev = after_inserted->prev;
    after_inserted->prev = newitem;
    if (after_inserted == array->child)
    {
        array->child = newitem;
    }
    else
    {
        newitem->prev->next = newitem;
    }
    return true;
}

CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemViaPointer(cJSON * const parent, cJSON * const item, cJSON * replacement)
{
    if ((parent == NULL) || (parent->child == NULL) || (replacement == NULL) || (item == NULL))
    {
        return false;
    }

    if (replacement == item)
    {
        return true;
    }

    replacement->next = item->next;
    replacement->prev = item->prev;

    if (replacement->next != NULL)
    {
        replacement->next->prev = replacement;
    }
    if (parent->child == item)
    {
        if (parent->child->prev == parent->child)
        {
            replacement->prev = replacement;
        }
        parent->child = replacement;
    }
    else
    {   /*
         * To find the last item in array quickly, we use prev in array.
         * We can't modify the last item's next pointer where this item was the parent's child
         */
        if (replacement->prev != NULL)
        {
            replacement->prev->next = replacement;
        }
        if (replacement->next == NULL)
        {
            parent->child->prev = replacement;
        }
    }

    item->next = NULL;
    item->prev = NULL;
    cJSON_Delete(item);

    return true;
}

CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInArray(cJSON *array, int which, cJSON *newitem)
{
    if (which < 0)
    {
        return false;
    }

    return cJSON_ReplaceItemViaPointer(array, get_array_item(array, (size_t)which), newitem);
}

static cJSON_bool replace_item_in_object(cJSON *object, const char *string, cJSON *replacement, cJSON_bool case_sensitive)
{
    if ((replacement == NULL) || (string == NULL))
    {
        return false;
    }

    /* replace the name in the replacement */
    if (!(replacement->type & cJSON_StringIsConst) && (replacement->string != NULL))
    {
        cJSON_free(replacement->string);
    }
    replacement->string = (char*)cJSON_strdup((const unsigned char*)string, &global_hooks);
    if (replacement->string == NULL)
    {
        return false;
    }

    replacement->type &= ~cJSON_StringIsConst;

    return cJSON_ReplaceItemViaPointer(object, get_object_item(object, string, case_sensitive), replacement);
}

CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInObject(cJSON *object, const char *string, cJSON *newitem)
{
    return replace_item_in_object(object, string, newitem, false);
}

CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInObjectCaseSensitive(cJSON *object, const char *string, cJSON *newitem)
{
    return replace_item_in_object(object, string, newitem, true);
}

/* Create basic types: */
CJSON_PUBLIC(cJSON *) cJSON_CreateNull(void)
{
    cJSON *item = cJSON_New_Item(&global_hooks);
    if(item)
    {
        item->type = cJSON_NULL;
    }

    return item;
}

CJSON_PUBLIC(cJSON *) cJSON_CreateTrue(void)
{
    cJSON *item = cJSON_New_Item(&global_hooks);
    if(item)
    {
        item->type = cJSON_True;
    }

    return item;
}

CJSON_PUBLIC(cJSON *) cJSON_CreateFalse(void)
{
    cJSON *item = cJSON_New_Item(&global_hooks);
    if(item)
    {
        item->type = cJSON_False;
    }

    return item;
}

CJSON_PUBLIC(cJSON *) cJSON_CreateBool(cJSON_bool boolean)
{
    cJSON *item = cJSON_New_Item(&global_hooks);
    if(item)
    {
        item->type = boolean ? cJSON_True : cJSON_False;
    }

    return item;
}

CJSON_PUBLIC(cJSON *) cJSON_CreateNumber(double num)
{
    cJSON *item = cJSON_New_Item(&global_hooks);
    if(item)
    {
        item->type = cJSON_Number;
        item->valuedouble = num;

        /* use saturation in case of overflow */
        if (num >= INT_MAX)
        {
            item->valueint = INT_MAX;
        }
        else if (num <= (double)INT_MIN)
        {
            item->valueint = INT_MIN;
        }
        else
        {
            item->valueint = (int)num;
        }
    }

    return item;
}

CJSON_PUBLIC(cJSON *) cJSON_CreateString(const char *string)
{
    cJSON *item = cJSON_New_Item(&global_hooks);
    if(item)
    {
        item->type = cJSON_String;
        item->valuestring = (char*)cJSON_strdup((const unsigned char*)string, &global_hooks);
        if(!item->valuestring)
        {
            cJSON_Delete(item);
            return NULL;
        }
    }

    return item;
}

CJSON_PUBLIC(cJSON *) cJSON_CreateStringReference(const char *string)
{
    cJSON *item = cJSON_New_Item(&global_hooks);
    if (item != NULL)
    {
        item->type = cJSON_String | cJSON_IsReference;
        item->valuestring = (char*)cast_away_const(string);
    }

    return item;
}

CJSON_PUBLIC(cJSON *) cJSON_CreateObjectReference(const cJSON *child)
{
    cJSON *item = cJSON_New_Item(&global_hooks);
    if (item != NULL) {
        item->type = cJSON_Object | cJSON_IsReference;
        item->child = (cJSON*)cast_away_const(child);
    }

    return item;
}

CJSON_PUBLIC(cJSON *) cJSON_CreateArrayReference(const cJSON *child) {
    cJSON *item = cJSON_New_Item(&global_hooks);
    if (item != NULL) {
        item->type = cJSON_Array | cJSON_IsReference;
        item->child = (cJSON*)cast_away_const(child);
    }

    return item;
}

CJSON_PUBLIC(cJSON *) cJSON_CreateRaw(const char *raw)
{
    cJSON *item = cJSON_New_Item(&global_hooks);
    if(item)
    {
        item->type = cJSON_Raw;
        item->valuestring = (char*)cJSON_strdup((const unsigned char*)raw, &global_hooks);
        if(!item->valuestring)
        {
            cJSON_Delete(item);
            return NULL;
        }
    }

    return item;
}

CJSON_PUBLIC(cJSON *) cJSON_CreateArray(void)
{
    cJSON *item = cJSON_New_Item(&global_hooks);
    if(item)
    {
        item->type=cJSON_Array;
    }

    return item;
}

CJSON_PUBLIC(cJSON *) cJSON_CreateObject(void)
{
    cJSON *item = cJSON_New_Item(&global_hooks);
    if (item)
    {
        item->type = cJSON_Object;
    }

    return item;
}

/* Create Arrays: */
CJSON_PUBLIC(cJSON *) cJSON_CreateIntArray(const int *numbers, int count)
{
    size_t i = 0;
    cJSON *n = NULL;
    cJSON *p = NULL;
    cJSON *a = NULL;

    if ((count < 0) || (numbers == NULL))
    {
        return NULL;
    }

    a = cJSON_CreateArray();

    for(i = 0; a && (i < (size_t)count); i++)
    {
        n = cJSON_CreateNumber(numbers[i]);
        if (!n)
        {
            cJSON_Delete(a);
            return NULL;
        }
        if(!i)
        {
            a->child = n;
        }
        else
        {
            suffix_object(p, n);
        }
        p = n;
    }

    if (a && a->child) {
        a->child->prev = n;
    }

    return a;
}

CJSON_PUBLIC(cJSON *) cJSON_CreateFloatArray(const float *numbers, int count)
{
    size_t i = 0;
    cJSON *n = NULL;
    cJSON *p = NULL;
    cJSON *a = NULL;

    if ((count < 0) || (numbers == NULL))
    {
        return NULL;
    }

    a = cJSON_CreateArray();

    for(i = 0; a && (i < (size_t)count); i++)
    {
        n = cJSON_CreateNumber((double)numbers[i]);
        if(!n)
        {
            cJSON_Delete(a);
            return NULL;
        }
        if(!i)
        {
            a->child = n;
        }
        else
        {
            suffix_object(p, n);
        }
        p = n;
    }

    if (a && a->child) {
        a->child->prev = n;
    }

    return a;
}

CJSON_PUBLIC(cJSON *) cJSON_CreateDoubleArray(const double *numbers, int count)
{
    size_t i = 0;
    cJSON *n = NULL;
    cJSON *p = NULL;
    cJSON *a = NULL;

    if ((count < 0) || (numbers == NULL))
    {
        return NULL;
    }

    a = cJSON_CreateArray();

    for(i = 0; a && (i < (size_t)count); i++)
    {
        n = cJSON_CreateNumber(numbers[i]);
        if(!n)
        {
            cJSON_Delete(a);
            return NULL;
        }
        if(!i)
        {
            a->child = n;
        }
        else
        {
            suffix_object(p, n);
        }
        p = n;
    }

    if (a && a->child) {
        a->child->prev = n;
    }

    return a;
}

CJSON_PUBLIC(cJSON *) cJSON_CreateStringArray(const char *const *strings, int count)
{
    size_t i = 0;
    cJSON *n = NULL;
    cJSON *p = NULL;
    cJSON *a = NULL;

    if ((count < 0) || (strings == NULL))
    {
        return NULL;
    }

    a = cJSON_CreateArray();

    for (i = 0; a && (i < (size_t)count); i++)
    {
        n = cJSON_CreateString(strings[i]);
        if(!n)
        {
            cJSON_Delete(a);
            return NULL;
        }
        if(!i)
        {
            a->child = n;
        }
        else
        {
            suffix_object(p,n);
        }
        p = n;
    }

    if (a && a->child) {
        a->child->prev = n;
    }

    return a;
}

/* Duplication */
CJSON_PUBLIC(cJSON *) cJSON_Duplicate(const cJSON *item, cJSON_bool recurse)
{
    cJSON *newitem = NULL;
    cJSON *child = NULL;
    cJSON *next = NULL;
    cJSON *newchild = NULL;

    /* Bail on bad ptr */
    if (!item)
    {
        goto fail;
    }
    /* Create new item */
    newitem = cJSON_New_Item(&global_hooks);
    if (!newitem)
    {
        goto fail;
    }
    /* Copy over all vars */
    newitem->type = item->type & (~cJSON_IsReference);
    newitem->valueint = item->valueint;
    newitem->valuedouble = item->valuedouble;
    if (item->valuestring)
    {
        newitem->valuestring = (char*)cJSON_strdup((unsigned char*)item->valuestring, &global_hooks);
        if (!newitem->valuestring)
        {
            goto fail;
        }
    }
    if (item->string)
    {
        newitem->string = (item->type&cJSON_StringIsConst) ? item->string : (char*)cJSON_strdup((unsigned char*)item->string, &global_hooks);
        if (!newitem->string)
        {
            goto fail;
        }
    }
    /* If non-recursive, then we're done! */
    if (!recurse)
    {
        return newitem;
    }
    /* Walk the ->next chain for the child. */
    child = item->child;
    while (child != NULL)
    {
        newchild = cJSON_Duplicate(child, true); /* Duplicate (with recurse) each item in the ->next chain */
        if (!newchild)
        {
            goto fail;
        }
        if (next != NULL)
        {
            /* If newitem->child already set, then crosswire ->prev and ->next and move on */
            next->next = newchild;
            newchild->prev = next;
            next = newchild;
        }
        else
        {
            /* Set newitem->child and move to it */
            newitem->child = newchild;
            next = newchild;
        }
        child = child->next;
    }
    if (newitem && newitem->child)
    {
        newitem->child->prev = newchild;
    }

    return newitem;

fail:
    if (newitem != NULL)
    {
        cJSON_Delete(newitem);
    }

    return NULL;
}

static void skip_oneline_comment(char **input)
{
    *input += static_strlen("//");

    for (; (*input)[0] != '\0'; ++(*input))
    {
        if ((*input)[0] == '\n') {
            *input += static_strlen("\n");
            return;
        }
    }
}

static void skip_multiline_comment(char **input)
{
    *input += static_strlen("/*");

    for (; (*input)[0] != '\0'; ++(*input))
    {
        if (((*input)[0] == '*') && ((*input)[1] == '/'))
        {
            *input += static_strlen("*/");
            return;
        }
    }
}

static void minify_string(char **input, char **output) {
    (*output)[0] = (*input)[0];
    *input += static_strlen("\"");
    *output += static_strlen("\"");


    for (; (*input)[0] != '\0'; (void)++(*input), ++(*output)) {
        (*output)[0] = (*input)[0];

        if ((*input)[0] == '\"') {
            (*output)[0] = '\"';
            *input += static_strlen("\"");
            *output += static_strlen("\"");
            return;
        } else if (((*input)[0] == '\\') && ((*input)[1] == '\"')) {
            (*output)[1] = (*input)[1];
            *input += static_strlen("\"");
            *output += static_strlen("\"");
        }
    }
}

CJSON_PUBLIC(void) cJSON_Minify(char *json)
{
    char *into = json;

    if (json == NULL)
    {
        return;
    }

    while (json[0] != '\0')
    {
        switch (json[0])
        {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                json++;
                break;

            case '/':
                if (json[1] == '/')
                {
                    skip_oneline_comment(&json);
                }
                else if (json[1] == '*')
                {
                    skip_multiline_comment(&json);
                } else {
                    json++;
                }
                break;

            case '\"':
                minify_string(&json, (char**)&into);
                break;

            default:
                into[0] = json[0];
                json++;
                into++;
        }
    }

    /* and null-terminate. */
    *into = '\0';
}

CJSON_PUBLIC(cJSON_bool) cJSON_IsInvalid(const cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == cJSON_Invalid;
}

CJSON_PUBLIC(cJSON_bool) cJSON_IsFalse(const cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == cJSON_False;
}

CJSON_PUBLIC(cJSON_bool) cJSON_IsTrue(const cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xff) == cJSON_True;
}


CJSON_PUBLIC(cJSON_bool) cJSON_IsBool(const cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & (cJSON_True | cJSON_False)) != 0;
}
CJSON_PUBLIC(cJSON_bool) cJSON_IsNull(const cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == cJSON_NULL;
}

CJSON_PUBLIC(cJSON_bool) cJSON_IsNumber(const cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == cJSON_Number;
}

CJSON_PUBLIC(cJSON_bool) cJSON_IsString(const cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == cJSON_String;
}

CJSON_PUBLIC(cJSON_bool) cJSON_IsArray(const cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == cJSON_Array;
}

CJSON_PUBLIC(cJSON_bool) cJSON_IsObject(const cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == cJSON_Object;
}

CJSON_PUBLIC(cJSON_bool) cJSON_IsRaw(const cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == cJSON_Raw;
}

CJSON_PUBLIC(cJSON_bool) cJSON_Compare(const cJSON * const a, const cJSON * const b, const cJSON_bool case_sensitive)
{
    if ((a == NULL) || (b == NULL) || ((a->type & 0xFF) != (b->type & 0xFF)))
    {
        return false;
    }

    /* check if type is valid */
    switch (a->type & 0xFF)
    {
        case cJSON_False:
        case cJSON_True:
        case cJSON_NULL:
        case cJSON_Number:
        case cJSON_String:
        case cJSON_Raw:
        case cJSON_Array:
        case cJSON_Object:
            break;

        default:
            return false;
    }

    /* identical objects are equal */
    if (a == b)
    {
        return true;
    }

    switch (a->type & 0xFF)
    {
        /* in these cases and equal type is enough */
        case cJSON_False:
        case cJSON_True:
        case cJSON_NULL:
            return true;

        case cJSON_Number:
            if (compare_double(a->valuedouble, b->valuedouble))
            {
                return true;
            }
            return false;

        case cJSON_String:
        case cJSON_Raw:
            if ((a->valuestring == NULL) || (b->valuestring == NULL))
            {
                return false;
            }
            if (strcmp(a->valuestring, b->valuestring) == 0)
            {
                return true;
            }

            return false;

        case cJSON_Array:
        {
            cJSON *a_element = a->child;
            cJSON *b_element = b->child;

            for (; (a_element != NULL) && (b_element != NULL);)
            {
                if (!cJSON_Compare(a_element, b_element, case_sensitive))
                {
                    return false;
                }

                a_element = a_element->next;
                b_element = b_element->next;
            }

            /* one of the arrays is longer than the other */
            if (a_element != b_element) {
                return false;
            }

            return true;
        }

        case cJSON_Object:
        {
            cJSON *a_element = NULL;
            cJSON *b_element = NULL;
            cJSON_ArrayForEach(a_element, a)
            {
                /* TODO This has O(n^2) runtime, which is horrible! */
                b_element = get_object_item(b, a_element->string, case_sensitive);
                if (b_element == NULL)
                {
                    return false;
                }

                if (!cJSON_Compare(a_element, b_element, case_sensitive))
                {
                    return false;
                }
            }

            /* doing this twice, once on a and b to prevent true comparison if a subset of b
             * TODO: Do this the proper way, this is just a fix for now */
            cJSON_ArrayForEach(b_element, b)
            {
                a_element = get_object_item(a, b_element->string, case_sensitive);
                if (a_element == NULL)
                {
                    return false;
                }

                if (!cJSON_Compare(b_element, a_element, case_sensitive))
                {
                    return false;
                }
            }

            return true;
        }

        default:
            return false;
    }
}

CJSON_PUBLIC(void *) cJSON_malloc(size_t size)
{
    return global_hooks.allocate(size);
}

CJSON_PUBLIC(void) cJSON_free(void *object)
{
    global_hooks.deallocate(object);
}
#pragma endregion CJSON_H

#pragma region RAY_H
#ifndef RAY_H
#define RAY_H
/**
 * @file ray.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module for rays.
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 * 
 */


typedef struct __attribute__((packed)) _Ray
{
    Vec3 origin;
    Vec3 direction;
} Ray;

#endif

#pragma endregion RAY_H

#pragma region CAMERA_H
#ifndef CAMERA_H
#define CAMERA_H
/**
 * @file camera.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module that represents a camera for looking at scenes.
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 * 
 */


typedef struct _Camera
{
    Vec3 position;
    Vec3 direction;
    Vec3 right;
    Vec3 up;
} Camera;

Camera *new_camera(Vec3 pos, Vec3 dir);

Ray *setup_perspective_rays(Camera *camera, u32 canvas_width, u32 canvas_height);

#endif
#include <math.h>

Camera *new_camera(Vec3 pos, Vec3 dir)
{
    Camera *camera = malloc(sizeof(Camera));
    camera->position = pos;
    camera->direction = dir;
    camera->right = norm(cross(camera->direction, vec3(0.0, 1.0, 0.0)));
    camera->up = norm(cross(camera->right, camera->direction));
    return camera;
}

#define PI 3.14159

Ray *setup_perspective_rays(Camera *camera, u32 canvas_width, u32 canvas_height)
{
    printf("Allocating a lot of rays, actually %u\n", canvas_height * canvas_width);
    Ray *rays = calloc(canvas_width * canvas_height, sizeof(Ray));
    f32 aspect_ratio = (f32)canvas_width / (f32)canvas_height;
    f32 fov = 90.0; // Field of view in degrees

    // Calculate the height and width of the image plane at distance 1
    f32 half_height = tanf(fov * 0.5 * PI / 180.0);
    f32 half_width = aspect_ratio * half_height;

    // Calculate the step sizes for moving between pixels on the image plane
    f32 step_x = 2.0 * half_width / (f32)(canvas_width - 1);
    f32 step_y = 2.0 * half_height / (f32)(canvas_height - 1);

    // Starting position on the top-left corner of the image plane
    // Vec3 image_plane_top_left =
    //     vsub(camera->position, vadd(smul(camera->direction, 1.0), smul(camera->up, half_height)));

    for (u32 y = 0; y < canvas_height; y++)
    {
        for (u32 x = 0; x < canvas_width; x++)
        {
            f32 sx = half_width - x * step_x;
            f32 sy = half_height - y * step_y;

            Vec3 ray_direction = norm(vadd(vadd(camera->direction, smul(camera->right, sx)), smul(camera->up, sy)));

            u32 ray_index = y * canvas_width + x;
            rays[ray_index].origin = camera->position;
            rays[ray_index].direction = ray_direction;
        }
    }
    return rays;
}

#pragma endregion CAMERA_H

#pragma region UTIL_H
#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

/**
 * @brief Checks if a char exists in a string.
 *
 * @param string The string to check.
 * @param character The char to look for.
 *
 * @return Whether the char is contained within the string.
 */
bool char_in_string(char* string, char character);

/**
 * @brief Replace all instances of a character in a string.
 *
 * @param string The string to replace in.
 * @param before The char to replace.
 * @param after The char to replace with.
 */
void char_replace(char* string, char before, char after);

char *read_file(char* path);

#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/**
 * @brief Checks if a char exists in a string.
 *
 * @param string The string to check.
 * @param character The char to look for.
 *
 * @return Whether the char is contained within the string.
 */
bool char_in_string(char* string, char character) {
    char* char_pos = strchr(string, character);
    return char_pos != NULL;
}

void char_replace(char* string, char before, char after) {
    char* char_pos = strchr(string, before);
    if (char_pos == NULL) {
        return;
    }
    while (char_pos) {
        *char_pos = after;
        char_pos = strchr(char_pos + 1, before);
    }
}

char *read_file(char *path)
{
    FILE *fp = fopen(path, "rb");
    fseek(fp, 0, SEEK_END);
    size_t fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *contents = malloc(fsize + 1);
    fread(contents, fsize, sizeof(char), fp);
    fclose(fp);
    contents[fsize] = '\0';
    return contents;
}

#pragma endregion UTIL_H

#pragma region LIST_H
#ifndef LIST_H
#define LIST_H
/**
 * @file list.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A generic list.
 * @version 0.1
 * @date 2023-08-27
 *
 * @copyright WingCorp (c) 2023
 *
 */

#define list_type(T)                                                                                                   \
    typedef struct __attribute__((packed)) _##T##List                                                                  \
    {                                                                                                                  \
        T *elements;                                                                                                   \
        u32 size;                                                                                                      \
        u32 capacity;                                                                                                  \
    } T##List;                                                                                                         \
                                                                                                                       \
    T##List *new_##T##List(u32 capacity)                                                                               \
    {                                                                                                                  \
        T##List *list = malloc(sizeof(T##List));                                                                       \
        if (list == NULL)                                                                                              \
        {                                                                                                              \
            failwithf("%sList_new: could not allocate more memory!\n", #T);                                            \
        }                                                                                                              \
        list->capacity = capacity;                                                                                     \
        list->size = 0;                                                                                                \
        list->elements = calloc(capacity, sizeof(T));                                                                  \
        if (list->elements == NULL)                                                                                    \
        {                                                                                                              \
            failwithf("%sList_new: could not allocate memory for initial capacity (%d)!\n", #T, capacity);             \
        }                                                                                                              \
        return list;                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    void destroy_##T##List(T##List *list)                                                                              \
    {                                                                                                                  \
        free(list->elements);                                                                                          \
        free(list);                                                                                                    \
    }                                                                                                                  \
                                                                                                                       \
    void T##List_add(T##List *list, T item)                                                                            \
    {                                                                                                                  \
        list->elements[list->size] = item;                                                                             \
        list->size++;                                                                                                  \
        if (list->size >= list->capacity)                                                                              \
        {                                                                                                              \
            list->capacity *= 2;                                                                                       \
            T *new_mem = realloc(list->elements, sizeof(T) * list->capacity);                                          \
            if (new_mem == NULL)                                                                                       \
            {                                                                                                          \
                failwithf("%sList_add: could not allocate more memory!\n", #T);                                        \
            }                                                                                                          \
            list->elements = new_mem;                                                                                  \
        }                                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    T T##List_get(T##List *list, u32 i)                                                                                \
    {                                                                                                                  \
        if (i < 0 || i > list->size)                                                                                   \
        {                                                                                                              \
            failwithf("%sList_set: index %u out of bounds (size: %d)!\n", #T, i, list->size);                          \
        }                                                                                                              \
        return list->elements[i];                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    void T##List_set(T##List *list, u32 i, T item)                                                                     \
    {                                                                                                                  \
        if (i < 0 || i > list->size)                                                                                   \
        {                                                                                                              \
            failwithf("%sList_set: index %u out of bounds (size: %d)!\n", #T, i, list->size);                          \
        }                                                                                                              \
        list->elements[i] = item;                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    u32 T##List_size(T##List *list)                                                                                    \
    {                                                                                                                  \
        return list->size;                                                                                             \
    }                                                                                                                  \
                                                                                                                       \
    void T##List_remove(T##List *list, u32 idx)                                                                        \
    {                                                                                                                  \
        if (idx < 0 || idx > list->size)                                                                               \
        {                                                                                                              \
            failwithf("%sList_remove: index %u out of bounds (size: %d)!\n", #T, idx, list->size);                     \
        }                                                                                                              \
        for (u32 i = idx + 1; i < list->size; i++)                                                                     \
        {                                                                                                              \
            list->elements[i - 1] = list->elements[i];                                                                 \
        }                                                                                                              \
        list->size--;                                                                                                  \
        if (list->capacity > (u32)(2.5 * list->size))                                                                  \
        {                                                                                                              \
            list->capacity = (u32)(1.5 * list->capacity) + 1;                                                          \
            T *new_mem = realloc(list->elements, sizeof(T) * list->capacity);                                          \
            if (new_mem == NULL)                                                                                       \
            {                                                                                                          \
                failwithf("%sList_remove: could not reduce assigned memory!\n", #T);                                   \
            }                                                                                                          \
            list->elements = new_mem;                                                                                  \
        }                                                                                                              \
    }

#define list_debug(ListT, print)                                                                                       \
    void ListT##_debug(ListT *list)                                                                                    \
    {                                                                                                                  \
        printf("[");                                                                                                   \
        for (u32 i = 0; i < list->size - 1; i++)                                                                       \
        {                                                                                                              \
            print(list->elements[i]);                                                                                  \
            printf(", ");                                                                                              \
        }                                                                                                              \
        print(list->elements[list->size - 1]);                                                                         \
        printf("]");                                                                                                   \
    }

#endif

#pragma endregion LIST_H

#pragma region COLOR_H
#ifndef COLOR_H
#define COLOR_H
/**
 * @file color.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module for using colors. Useful when you want to compose colors later on I guess.
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 * 
 */

typedef Vec3 Color;

Color color_mix(Color a, Color b);
void color_register_format(void *fmt, u32 (*fun)(void *fmt, u8 r, u8 g, u8 b));
u32 color_to_pixel(Color color);

#endif
#include <math.h>

static void *global_format = NULL;
// Uint32 SDL_MapRGB(const SDL_PixelFormat *format, Uint8 r, Uint8 g, Uint8 b)
static u32 (*pixel_fun)(void *fmt, u8 r, u8 g, u8 b) = NULL;

void color_register_format(void *fmt, u32 (*fun)(void *fmt, u8 r, u8 g, u8 b))
{
    global_format = fmt;
    pixel_fun = fun;
}

u32 color_to_pixel(Color color)
{
    if (global_format == NULL) failwith("Color pixelformat not set!");
    if (pixel_fun == NULL) failwith("Color pixelformat not set!");
    Color nc = vabs(norm(color));
    return pixel_fun(global_format, (u8)truncf(nc.x * 255.0f), (u8)truncf(nc.y * 255.0f), (u8)truncf(nc.z * 255.0f));
}

Color color_mix(Color a, Color b)
{
    return vec3(
        (1.0 - b.x) * a.x + b.x,
        (1.0 - b.y) * a.y + b.y,
        (1.0 - b.z) * a.z + b.z
    );
}

#pragma endregion COLOR_H

#pragma region LIGHT_H
#ifndef LIGHT_H
#define LIGHT_H
/**
 * @file light.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A point light implementation
 * @version 0.1
 * @date 2023-08-27
 *
 * @copyright WingCorp (c) 2023
 * 
 */


typedef struct __attribute__((packed)) _Light
{
    Vec3 position;
    Color color;
} Light;

#endif

#pragma endregion LIGHT_H

#pragma region HIT_H
#ifndef HIT_H
#define HIT_H
/**
 * @file hit.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module for 'Hit' structs.
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 * 
 */


typedef struct __attribute__((packed)) _Hit
{
    Color color;
    f32 distance;
    Vec3 position;
    Vec3 norm;
} Hit;

option_type(Hit);

#endif


#pragma endregion HIT_H

#pragma region SPHERE_H
#ifndef SPHERE_H
#define SPHERE_H
/**
 * @file sphere.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module for spheres.
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 * 
 */


typedef struct _Sphere
{
    Vec3 center;
    f32 radius;
    Color color;
} Sphere;

Sphere* new_sphere(Vec3 center, f32 radius, Color color);

HitOption sphere_intersect(Sphere *sphere, Ray *ray);

#endif

#include <math.h>

Sphere *new_sphere(Vec3 center, f32 radius, Color color)
{
    Sphere *sphere = malloc(sizeof(Sphere));
    sphere->center = center;
    sphere->radius = radius;
    sphere->color = color;
    return sphere;
}

/**
 * @brief Compute the intersect between a sphere and a ray.
 *
 * @param sphere A sphere.
 * @param ray A ray.
 * @return HitOption An option of the Hit-persuasion.
 */
HitOption sphere_intersect(Sphere *sphere, Ray *ray)
{
    f32 radius2 = (sphere->radius * sphere->radius);
    Vec3 center_dir = vsub(sphere->center, ray->origin);
    f32 origin_to_center = dot(center_dir, ray->direction);
    
    if (origin_to_center < 0.0f) return no_Hit();

    f32 offset = dot(center_dir, center_dir) - (origin_to_center * origin_to_center);
    if (offset > (radius2 + eps)) return no_Hit();
    
    f32 hit_to_center = sqrtf(radius2 - offset);
    f32 distance = min(origin_to_center - hit_to_center, origin_to_center + hit_to_center);
    if (distance < 0.0f)
    {
        return no_Hit();
    }

    Vec3 hit_point = vadd(ray->origin, smul(ray->direction, distance));
    Vec3 hit_norm = norm(vsub(hit_point, sphere->center));
    Vec3 hit_to_center_vec = norm(vsub(sphere->center, hit_point));
    if (dot(hit_norm, hit_to_center_vec) < 0.1) {
        hit_norm = smul(hit_norm, -1.0f);
    }
    return some_Hit((Hit) {
        .color = sphere->color,
        .distance = dist(ray->origin, hit_point),
        .position = hit_point,
        .norm = hit_norm
    });
}

#pragma endregion SPHERE_H

#pragma region PLANE_H
#ifndef PLANE_H
#define PLANE_H
/**
 * @file plane.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief 
 * @version 0.1
 * @date 2023-09-05
 *
 * @copyright WingCorp (c) 2023
 * 
 */


typedef struct _Plane
{
    Vec3 pivot;
    Vec3 normal;
    Color color;
} Plane;

Plane *new_plane(Vec3 pivot, Vec3 normal, Color color);

HitOption plane_intersect(Plane *plane, Ray *ray);
#endif
#include <math.h>

Plane * new_plane(Vec3 pivot, Vec3 normal, Color color) {
    Plane *plane = malloc(sizeof(Plane));
    plane->pivot = pivot;
    plane->normal = norm(normal);
    plane->color = color;
    return plane;
}

HitOption plane_intersect(Plane * plane, Ray * ray) {
    f32 d = dot(plane->normal, ray->direction);
    if (fabsf(d) < eps) {
        return no_Hit();
    }

    Vec3 pivot_minus_origin = vsub(plane->pivot, ray->origin);
    f32 t = dot(pivot_minus_origin, plane->normal) / d;

    if (t < 0) {
        return no_Hit();
    }

    Vec3 hit_position = vadd(ray->origin, smul(ray->direction, t));
    return some_Hit((Hit) {
        .distance = dist(ray->origin, hit_position),
        .position = hit_position,
        .norm = plane->normal,
        .color = plane->color
    });
}

#pragma endregion PLANE_H

#pragma region SCENE_H
#ifndef SCENE_H
#define SCENE_H
/**
 * @file scene.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief Contains a scene.
 * @version 0.1
 * @date 2023-08-27
 *
 * @copyright WingCorp (c) 2023
 * 
 */


typedef struct _Scene Scene;

Scene* new_scene();
Camera *scene_get_camera(Scene *scene);
void scene_set_camera(Scene *scene, Camera *camera);
void scene_add_sphere(Scene *scene, Sphere *sphere);
void scene_add_plane(Scene *scene, Plane *plane);
void scene_add_light(Scene *scene, Light light);
HitOption trace_ray(Scene *scene, Ray *ray);
void scene_free(Scene *scene);
void scene_debug_print(Scene *scene);
#endif

#ifndef SpherePtrList_T
typedef Sphere *SpherePtr;
list_type(SpherePtr);
#define SpherePtrList_T
#endif
#ifndef LightList_T
list_type(Light);
#define LightList_T
#endif
typedef Plane* PlanePtr;
#ifndef PlanePtrList_T
#define PlanePtrList_T
list_type(PlanePtr);
#endif

typedef struct _Scene
{
    Camera *camera;
    LightList *lights;
    SpherePtrList *spheres;
    PlanePtrList *planes;
} Scene;

Scene *new_scene()
{
    Scene *s = malloc(sizeof(Scene));
    s->camera = NULL;
    s->lights = new_LightList(4);
    s->spheres = new_SpherePtrList(8);
    s->planes = new_PlanePtrList(3);
    return s;
}

void scene_set_camera(Scene *scene, Camera *camera)
{
    scene->camera = camera;
}

void scene_add_sphere(Scene *scene, Sphere *sphere)
{
    SpherePtrList_add(scene->spheres, sphere);
}

void scene_add_light(Scene *scene, Light light)
{
    LightList_add(scene->lights, light);
}

void scene_add_plane(Scene *scene, Plane *plane) {
    PlanePtrList_add(scene->planes, plane);
}

static HitOption cast_ray(Scene *scene, Ray *ray)
{
    if (scene->camera == NULL) {
        failwith("No camera set in scene, cannot cast rays!\n");
    }
    HitOption closest_ = no_Hit();
    f32 closest_dist = 999999.0f;
    for (u16 i = 0; i < scene->spheres->size; i++)
    {
        Sphere *s = scene->spheres->elements[i];
        HitOption current_ = sphere_intersect(s, ray);
        if (is_some(current_) && current_.value.distance <= closest_dist)
        {
            closest_dist = current_.value.distance;
            closest_ = current_;
        }
    }
    for (u16 i = 0; i < scene->planes->size; i++)
    {
        Plane *p = scene->planes->elements[i];
        HitOption current_ = plane_intersect(p, ray);
        if (is_some(current_) && current_.value.distance <= closest_dist) {
            closest_dist = current_.value.distance;
            closest_ = current_;
        }
    }
    return closest_;
}

HitOption trace_ray(Scene *scene, Ray *ray)
{
    if (scene->camera == NULL) {
        failwith("No camera set in scene, cannot cast rays!\n");
    }
    HitOption closest_ = cast_ray(scene, ray);
    if (is_some(closest_))
    {
        // Compute the color depending on whether we are in darkness or not.
        bool reached_by_light = false;
        Ray shadow_ray;
        shadow_ray.origin = vadd(closest_.value.position, smul(closest_.value.norm, 0.01));
        for (u16 i = 0; i < scene->lights->size; i++)
        {
            Light l = scene->lights->elements[i];
            shadow_ray.direction = norm(vsub(l.position, shadow_ray.origin));
            HitOption shadow_hit_ = cast_ray(scene, &shadow_ray);
            if (is_none(shadow_hit_))
            {
                reached_by_light = true;
                closest_.value.color = closest_.value.color;
            }
        }
        if (!reached_by_light)
        {
            closest_.value.color = vec3(0.0, 0.0, 0.0);
        }
    }
    return closest_;
}

void scene_free(Scene *scene)
{
    destroy_SpherePtrList(scene->spheres);
    destroy_LightList(scene->lights);
    free(scene);
}

void scene_debug_print(Scene *scene)
{
    printf("Scene debug:\n");
    printf(
        "\tCamera: { p: (%.2f, %.2f, %.2f), d: (%.2f, %.2f, %.2f) }\n",
        scene->camera->position.x,
        scene->camera->position.y,
        scene->camera->position.z,
        scene->camera->direction.x,
        scene->camera->direction.y,
        scene->camera->direction.z
    );
    for(u32 i = 0; i < scene->lights->size; i++) {
        printf(
            "\tLight: { o: (%.2f, %.2f, %.2f), c: (%.2f, %.2f, %.2f) }\n",
            scene->lights->elements[i].position.x,
            scene->lights->elements[i].position.y,
            scene->lights->elements[i].position.z,
            scene->lights->elements[i].color.x,
            scene->lights->elements[i].color.y,
            scene->lights->elements[i].color.z
        );
    }
    printf("Scene has %u spheres:\n", scene->spheres->size);
    for(u32 i = 0; i < scene->spheres->size; i++) {
        printf(
            "\tSphere: { o: (%.2f, %.2f, %.2f), r: %.2f, c: (%.2f, %.2f, %.2f) }\n",
            scene->spheres->elements[i]->center.x,
            scene->spheres->elements[i]->center.y,
            scene->spheres->elements[i]->center.z,
            scene->spheres->elements[i]->radius,
            scene->spheres->elements[i]->color.x,
            scene->spheres->elements[i]->color.y,
            scene->spheres->elements[i]->color.z
        );
    }
    printf("Scene has %u planes:\n", scene->planes->size);
    for(u32 i = 0; i < scene->planes->size; i++) {
        printf(
            "\tPlane: { p: (%.2f, %.2f, %.2f), n: (%.2f, %.2f, %.2f), c: (%.2f, %.2f, %.2f) }\n",
            scene->planes->elements[i]->pivot.x,
            scene->planes->elements[i]->pivot.y,
            scene->planes->elements[i]->pivot.z,
            scene->planes->elements[i]->normal.x,
            scene->planes->elements[i]->normal.y,
            scene->planes->elements[i]->normal.z,
            scene->planes->elements[i]->color.x,
            scene->planes->elements[i]->color.y,
            scene->planes->elements[i]->color.z
        );
    }
}

Camera *scene_get_camera(Scene *scene)
{
    if(scene->camera != NULL) {
        return scene->camera;
    }
    failwith("Tried to get camera from scene, but none was available :(\n");
}

#pragma endregion SCENE_H

#pragma region PARSER_H
#ifndef SCENE_PARSER_H
#define SCENE_PARSER_H
/**
 * @file parser.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module that parses scenes from scene json-files
 * @version 0.1
 * @date 2023-08-27
 *
 * @copyright WingCorp (c) 2023
 * 
 */


Scene *parse_scene(char *path);

#endif
#include <math.h>
#include <string.h>

Vec3 parse_vec3(cJSON *vector)
{
    if (vector == NULL) {
        failwith("Cannot parse vector, it was a null pointer!\n");
    }
    if (cJSON_HasObjectItem(vector, "x"))
    {
        return vec3(cJSON_GetObjectItem(vector, "x")->valuedouble, cJSON_GetObjectItem(vector, "y")->valuedouble,
                    cJSON_GetObjectItem(vector, "z")->valuedouble);
    }
    if (cJSON_HasObjectItem(vector, "r"))
    {
        return vec3(cJSON_GetObjectItem(vector, "r")->valuedouble, cJSON_GetObjectItem(vector, "g")->valuedouble,
                    cJSON_GetObjectItem(vector, "b")->valuedouble);
    }
    failwith("Could not parse vector!\n");
}

Camera *parse_camera(cJSON *root)
{
    cJSON *camera = cJSON_GetObjectItem(root, "camera");
    if (camera == NULL)
    {
        failwith("Failed to parse scene from JSON: No camera object found!\n");
    }
    cJSON *position = cJSON_GetObjectItem(camera, "position");
    if (position == NULL)
    {
        failwith("Camera had no position vector!\n");
    }
    cJSON *direction = cJSON_GetObjectItem(camera, "direction");
    if (direction == NULL)
    {
        failwith("Camera had no direction vector!\n");
    }
    return new_camera(parse_vec3(position), parse_vec3(direction));
}

void parse_lights(Scene *scene, cJSON *root)
{
    cJSON *lights = cJSON_GetObjectItem(root, "lights");
    if (lights == NULL)
    {
        fprintf(stderr, "Warning, no lights parsed from scene ...\n");
    }
    int lights_count = cJSON_GetArraySize(lights);
    for (u32 i = 0; i < lights_count; i++)
    {
        cJSON *light = cJSON_GetArrayItem(lights, i);
        cJSON *position = cJSON_GetObjectItem(light, "position");
        if (position == NULL)
            continue;
        cJSON *color = cJSON_GetObjectItem(light, "color");
        if (color == NULL)
            continue;
        scene_add_light(scene, (Light){.position = parse_vec3(position), .color = parse_vec3(color)});
    }
}

void parse_shapes(Scene *scene, cJSON *root)
{
    cJSON *shapes = cJSON_GetObjectItem(root, "shapes");
    if (shapes == NULL)
    {
        return;
    }
    u32 shapes_count = cJSON_GetArraySize(shapes);
    for (int i = 0; i < shapes_count; i++)
    {
        cJSON *shape = cJSON_GetArrayItem(shapes, i);

        cJSON *type = cJSON_GetObjectItem(shape, "type");

        if (strcmp(type->valuestring, "sphere") == 0) {
            cJSON *color = cJSON_GetObjectItem(shape, "color");
            cJSON *radius = cJSON_GetObjectItem(shape, "radius");
            cJSON *center = cJSON_GetObjectItem(shape, "center");

            Sphere *sphere = new_sphere(parse_vec3(center), radius->valuedouble, parse_vec3(color));
            scene_add_sphere(scene, sphere);
        }

        if (strcmp(type->valuestring, "plane") == 0) {
            cJSON *color = cJSON_GetObjectItem(shape, "color");
            cJSON *normal = cJSON_GetObjectItem(shape, "normal");
            cJSON *pivot = cJSON_GetObjectItem(shape, "pivot");
            
            Plane *plane = new_plane(parse_vec3(pivot), parse_vec3(normal), parse_vec3(color));
            scene_add_plane(scene, plane);
        }
    }
}

Scene *parse_scene(char *path)
{
    char *contents = read_file(path);
    cJSON *root = cJSON_Parse(contents);
    if (root == NULL)
    {
        const char *error = cJSON_GetErrorPtr();
        if (error != NULL)
        {
            cJSON_Delete(root);
            failwithf("Error parsing JSON: %s\n", error);
        }
    }
    Scene *scene = new_scene();
    Camera *camera = parse_camera(root);
    scene_set_camera(scene, camera);
    parse_lights(scene, root);
    parse_shapes(scene, root);
    free(contents);
    return scene;
}
#pragma endregion PARSER_H

#pragma region MAIN_C
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <math.h>
#ifdef __linux__
#include <unistd.h>
#else
#include <getopt.h>
#endif

static u8 cpu_count = 12;
static u8 batch_size = 3;

typedef struct _RayWorkerArgs
{
    u16 id;
    SDL_Surface *canvas;
    u64 offset;
    u64 amount;
    Ray *rays;
    Scene *scene;
} RayWorkerArgs;

int fire_rays(void *args)
{
    RayWorkerArgs *wargs = (RayWorkerArgs *)args;

    SDL_Surface *canvas = wargs->canvas;

    Ray *rays = wargs->rays;
    Scene *scene = wargs->scene;

    u64 offset = wargs->offset;
    u64 amount = wargs->amount;

    u32 *raster = canvas->pixels;

    HitOption hit_;
    printf("rw[%hu]: Firing %llu rays.\n", wargs->id, amount);
    for (u32 i = offset; i < offset + amount; i += batch_size)
    {
        SDL_LockSurface(canvas);
        for (u32 j = 0; j < batch_size; j++)
        {
            hit_ = trace_ray(scene, rays + i + j);
            if (is_some(hit_))
            {
                raster[i + j] = color_to_pixel(hit_.value.color);
            }
        }
        SDL_UnlockSurface(canvas);
    }
    printf("rw[%hu]: Done!\n", wargs->id);
    return 0;
}

typedef struct _RenderArgs
{
    Scene *scene;
    SDL_atomic_t *running;
    SDL_atomic_t *buffer_switched;
    SDL_Surface *canvas;
    SDL_Surface *window_surface;
    SDL_Window *window;
} RenderArgs;

int render(void *args)
{
    RenderArgs *rargs = (RenderArgs *)args;

    SDL_atomic_t *running = rargs->running;
    SDL_atomic_t *buffer_switched = rargs->buffer_switched;
    SDL_Window *window = rargs->window;
    SDL_Surface *window_surface = rargs->window_surface;
    SDL_Surface *canvas = rargs->canvas;

    Scene *scene = rargs->scene;
    Camera *camera = scene_get_camera(scene); // new_camera(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0));
    Ray *rays = setup_perspective_rays(camera, canvas->w, canvas->h);

    SDL_Thread **workers = calloc(cpu_count, sizeof(SDL_Thread *));
    RayWorkerArgs **wargs = calloc(cpu_count, sizeof(RayWorkerArgs *));
    u64 canvas_size = canvas->w * canvas->h;
    u64 parallelism = canvas_size / ((u64)cpu_count);
    printf("Parallelism: %llu rays per thread, %llu rays in total.\n", parallelism, canvas_size);
    for (u32 i = 0; i < cpu_count; i++)
    {
        RayWorkerArgs *rwargs = malloc(sizeof(RayWorkerArgs));
        rwargs->id = i;
        rwargs->offset = i * parallelism;
        rwargs->amount = i < (cpu_count - 1) ? parallelism : canvas_size - (parallelism * i);
        rwargs->canvas = canvas;
        rwargs->rays = rays;
        rwargs->scene = scene;
        wargs[i] = rwargs;
        workers[i] = SDL_CreateThread(fire_rays, NULL, rwargs);
    }

#define render_surface()                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        if (SDL_AtomicGet(buffer_switched))                                                                            \
        {                                                                                                              \
            window_surface = SDL_GetWindowSurface(window);                                                             \
            SDL_BlitScaled(canvas, NULL, window_surface, NULL);                                                        \
            SDL_AtomicSet(buffer_switched, false);                                                                     \
        }                                                                                                              \
        SDL_BlitScaled(canvas, NULL, window_surface, NULL);                                                            \
        SDL_UpdateWindowSurface(window);                                                                               \
    } while (0)

    while (SDL_AtomicGet(running))
    {
        render_surface();
    }
    for (u32 i = 0; i < cpu_count; i++)
    {
        SDL_WaitThread(workers[i], NULL);
    }
    free(rays);
    free(workers);
    for (u32 i = 0; i < cpu_count; i++)
    {
        free(wargs[i]);
    }
    free(wargs);
    render_surface();
#undef render_surface
    return 0;
}

int main(int argc, char *argv[])
{
    int opt;
    u32 window_w = 1792, w = 1792, window_h = 768, h = 768;
    char *input_file = NULL;
    bool debug = false;

    while ((opt = getopt(argc, argv, "w:h:c:b:d:i:")) != -1)
    {
        switch (opt)
        {
        case 'w':
            w = atoi(optarg);
            break;
        case 'h':
            h = atoi(optarg);
            break;
        case 'c':
            cpu_count = atoi(optarg);
            break;
        case 'b':
            batch_size = atoi(optarg);
            break;
        case 'd':
            debug = true;
            break;
        case 'i':
            input_file = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s [-w width] [-h height] [-c cpu_count] [-b batch_size] [-d] -i <input_scene.json>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (input_file == NULL)
    {
        fprintf(stderr, "An input file path is required.\n");
        fprintf(stderr, "Usage: %s [-w width] [-h height] [-c cpu_count] [-b batch_size] [-d] -i <input_scene.json>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    Scene *scene = parse_scene(input_file);
    if (debug) {
        scene_debug_print(scene);
    }
    SDL_atomic_t *running = malloc(sizeof(SDL_atomic_t));
    running->value = true;
    SDL_atomic_t *buffer_switched = malloc(sizeof(SDL_atomic_t));
    buffer_switched->value = false;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        failwith("Could not initialize SDL2 library!\n");
    }
    SDL_Window *window = SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_w,
                                          window_h, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
    {
        failwith("Window not opened, failing...\n");
    }
    SDL_Surface *window_surface = SDL_GetWindowSurface(window);
    if (window_surface == NULL)
    {
        failwith("Failed to retrieve window surface!\n");
    }
    SDL_Surface *canvas = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGB888);
    SDL_PixelFormat *fmt = canvas->format;
    // Awful casting, but silences the warnings.
    color_register_format(fmt, (u32(*)(void *, u8, u8, u8)) & SDL_MapRGB);
    printf("color (1.0, 1.0, 1.0) -> %u\n", color_to_pixel(vec3(1.0, 1.0, 1.0)));
    RenderArgs *rargs = malloc(sizeof(RenderArgs));
    rargs->buffer_switched = buffer_switched;
    rargs->running = running;
    rargs->canvas = canvas;
    rargs->window_surface = window_surface;
    rargs->window = window;
    rargs->scene = scene;
    SDL_Thread *render_thread = SDL_CreateThread(render, "RENDER", rargs);
    SDL_Event e;
    while (SDL_AtomicGet(running))
    {
        while (SDL_PollEvent(&e) > 0)
        {
            if (e.type == SDL_QUIT)
            {
                SDL_AtomicSet(running, false);
                break;
            }
            SDL_AtomicSet(buffer_switched, true);
        }
    }
    SDL_WaitThread(render_thread, NULL);
    free(rargs);
    scene_free(scene);
    return 0;
}


#pragma endregion MAIN_C
