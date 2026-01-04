#pragma once
#include <stdint.h>
#include <stdbool.h>

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

#define var __auto_type
#define cnst const var

#ifndef max
#define max(a, b) ((a) > (b)? (a): (b))
#endif

#ifndef min
#define min(a, b) ((a) < (b)? (a): (b))
#endif