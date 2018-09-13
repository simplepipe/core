#ifndef __kernel_def_h
#define __kernel_def_h

#include <stdint.h>
#include <inttypes.h>

#define KEYSIZE_L(p) p, sizeof(p)-1
#define KEYSIZE_P(p) (void *)&(p), sizeof(p)
#define KEYSIZE_C(p) p, strlen(p)

typedef int8_t          i8;
typedef int16_t         i16;
typedef int32_t         i32;
typedef int64_t         i64;

typedef uint8_t         u8;
typedef uint16_t        u16;
typedef uint32_t        u32;
typedef uint64_t        u64;

#endif
