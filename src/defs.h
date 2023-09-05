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