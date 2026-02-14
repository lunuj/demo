#pragma once
#include <stdint.h>
#include <string.h>

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

#define ARRARY_LEN(array) (sizeof(array)/sizeof(array[0]))
#define SWAP(a, b) do { b = a ^ b; a = b ^ a; b = a ^ b; } while (0)
#define UNUSED(x) (void)(x)
#define MIN(a, b) ( a < b ? a : b )
#define MAX(a, b) ( a > b ? a : b )

#define SET_BIT(reg, bit) ((reg) |= (1 << (bit)))
#define CLEAR_BIT(reg, bit) ((reg) &= ~(1 << (bit)))
#define TEST_BIT(reg, bit) (((reg) & (1 << (bit))) != 0)
#define TOGGLE_BIT(reg, bit) ((reg) ^= (1 << (bit)))
// #define shift(xs, xs_sz) (NOB_ASSERT((xs_sz) > 0), (xs_sz)--, *(xs)++)