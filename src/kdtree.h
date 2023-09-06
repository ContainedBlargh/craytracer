#ifndef KDTREE_H
#define KDTREE_H
/**
 * @file kdtree.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A macro for making KDTrees lmao
 * @version 0.1
 * @date 2023-09-06
 *
 * @copyright WingCorp (c) 2023
 *
 */
#include "vec3.h"
#include "defs.h"

#define define_V3Tree(T, vec3Expr)         \
    typedef struct _##T##V3Node T##V3Node; \
                                           \
    typedef struct _##T##V3Node {          \
        u8 axis;                           \
        Vec3 max;                          \
        Vec3 min;                          \
        T##V3Node *left;                   \
        T##V3Node *right;                  \
    } T##V3Node;                           \
                                           \
    typedef struct _##T##V3Tree {          \
        T##V3Node *root;                   \
        
    } T##V3Tree;

#endif