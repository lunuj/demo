#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

#define RET_OK 0
#define RET_ERR -1

#define ARRARY_LEN(array) (sizeof(array) / sizeof(array[0]))
#define SWAP(a, b)     \
    do {               \
        b = (a) ^ (b); \
        a = (b) ^ (a); \
        b = (a) ^ (b); \
    } while (0)

#define SWAP_T(T, a, b) \
    do {                \
        T t = a;        \
        a = b;          \
        b = t;          \
    } while (0)

#define UNUSED(x) (void)(x)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define SET_BIT(reg, bit) ((reg) |= (1 << (bit)))
#define CLEAR_BIT(reg, bit) ((reg) &= ~(1 << (bit)))
#define TEST_BIT(reg, bit) (((reg) & (1 << (bit))) != 0)
#define TOGGLE_BIT(reg, bit) ((reg) ^= (1 << (bit)))

#define TODO(message)                                                      \
    do {                                                                   \
        fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message); \
        abort();                                                           \
    } while (0)
#define shift(xs, xs_sz) (assert((xs_sz) > 0), (xs_sz)--, *(xs)++)

static inline float Q_rsqrt(float number)
{
    static_assert(sizeof(float) == sizeof(uint32_t),
                  "AST_RSQRT does not work on this architecture");

    float x = number * 0.5f;
    float y = number;
    float threehalfs = 1.5f;

    uint32_t i = *(uint32_t *)&number;

    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;

    y = y * (threehalfs - x * y * y);

    return y;
}