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
#include "defs.h"
#include "fail.h"

#define list_type(T)                                                            \
    typedef struct __attribute__((packed)) _##T##List {                         \
        T *elements;                                                            \
        u32 size;                                                               \
        u32 capacity;                                                           \
    } T##List;                                                                  \
                                                                                \
    T##List *new_##T##List(u32 capacity) {                                      \
        T##List *list = malloc(sizeof(T##List));                                \
        if (list == NULL) {                                                     \
            failwithf("%sList_new: could not allocate more memory!\n", #T);     \
        }                                                                       \
        list->capacity = capacity;                                              \
        list->size = 0;                                                         \
        list->elements = calloc(capacity, sizeof(T));                           \
        if (list->elements == NULL) {                                           \
            failwithf(                                                          \
                "%sList_new: could not allocate memory for initial capacity "   \
                "(%d)!\n",                                                      \
                #T, capacity);                                                  \
        }                                                                       \
        return list;                                                            \
    }                                                                           \
                                                                                \
    void destroy_##T##List(T##List *list) {                                     \
        free(list->elements);                                                   \
        free(list);                                                             \
    }                                                                           \
                                                                                \
    void T##List_add(T##List *list, T item) {                                   \
        list->elements[list->size] = item;                                      \
        list->size++;                                                           \
        if (list->size >= list->capacity) {                                     \
            list->capacity *= 2;                                                \
            T *new_mem = realloc(list->elements, sizeof(T) * list->capacity);   \
            if (new_mem == NULL) {                                              \
                failwithf("%sList_add: could not allocate more memory!\n", #T); \
            }                                                                   \
            list->elements = new_mem;                                           \
        }                                                                       \
    }                                                                           \
                                                                                \
    T T##List_get(T##List *list, u32 i) {                                       \
        if (i < 0 || i > list->size) {                                          \
            failwithf("%sList_set: index %u out of bounds (size: %d)!\n", #T,   \
                i, list->size);                                                 \
        }                                                                       \
        return list->elements[i];                                               \
    }                                                                           \
                                                                                \
    void T##List_set(T##List *list, u32 i, T item) {                            \
        if (i < 0 || i > list->size) {                                          \
            failwithf("%sList_set: index %u out of bounds (size: %d)!\n", #T,   \
                i, list->size);                                                 \
        }                                                                       \
        list->elements[i] = item;                                               \
    }                                                                           \
                                                                                \
    u32 T##List_size(T##List *list) {                                           \
        return list->size;                                                      \
    }                                                                           \
                                                                                \
    void T##List_remove(T##List *list, u32 idx) {                               \
        if (idx < 0 || idx > list->size) {                                      \
            failwithf("%sList_remove: index %u out of bounds (size: %d)!\n",    \
                #T, idx, list->size);                                           \
        }                                                                       \
        for (u32 i = idx + 1; i < list->size; i++) {                            \
            list->elements[i - 1] = list->elements[i];                          \
        }                                                                       \
        list->size--;                                                           \
        if (list->capacity > (u32)(2.5 * list->size)) {                         \
            list->capacity = (u32)(1.5 * list->capacity) + 1;                   \
            T *new_mem = realloc(list->elements, sizeof(T) * list->capacity);   \
            if (new_mem == NULL) {                                              \
                failwithf(                                                      \
                    "%sList_remove: could not reduce assigned memory!\n", #T);  \
            }                                                                   \
            list->elements = new_mem;                                           \
        }                                                                       \
    }

/**
 * @brief A macro that defines a sort function for you list.
 * @param T The type that you used for your underlying list.
 * @param keyName The name of what the list is being sorted by.
 * @param keyFun A macro or function that takes one of your T's and returns an
 * 32-bit integer key.
 */
#define list_define_sort_by(T, keyName, keyFun)                               \
                                                                              \
    int _P##T##By##keyName##FuncCompare(const void *a, const void *b) {       \
        T ta = *(T *)a;                                                       \
        T tb = *(T *)b;                                                       \
        return keyFun(ta) - keyFun(tb);                                       \
    }                                                                         \
                                                                              \
    void T##List##_sort_by_##keyName(T##List *list) {                         \
        T *elements = realloc(list->elements, sizeof(T) * list->size);        \
        if (elements == NULL) {                                               \
            failwith("Realloc of list for sorting returned NULL ptr!\n");     \
        }                                                                     \
        qsort(elements, list->size, sizeof(T),                                \
            &_P##T##By##keyName##FuncCompare);                                \
        T *full_capacity = realloc(elements, sizeof(T) * list->capacity);     \
        if (full_capacity == NULL) {                                          \
            failwith("Realloc of list for sort result returned NULL ptr!\n"); \
        }                                                                     \
        list->elements = full_capacity;                                       \
    }

#define list_debug(T, print)                       \
    void T##List##_debug(T##List *list) {          \
        printf("[");                               \
        for (u32 i = 0; i < list->size - 1; i++) { \
            print(list->elements[i]);              \
            printf(", ");                          \
        }                                          \
        print(list->elements[list->size - 1]);     \
        printf("]");                               \
    }

#endif