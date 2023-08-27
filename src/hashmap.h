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
#include "defs.h"

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
