/*
 *  xxHash - Fast Hash algorithm
 *  Copyright (C) 2012-2016, Yann Collet
 *
 *  BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *  * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You can contact the author at :
 *  - xxHash homepage: http://www.xxhash.com
 *  - xxHash source repository : https://github.com/Cyan4973/xxHash
 */

/* *************************************
 *  Tuning parameters
 ***************************************/
/*!XXH_FORCE_MEMORY_ACCESS :
 * By default, access to unaligned memory is controlled by `memcpy()`, which is safe and portable.
 * Unfortunately, on some target/compiler combinations, the generated assembly is sub-optimal.
 * The below switch allow to select different access method for improved performance.
 * Method 0 (default) : use `memcpy()`. Safe and portable.
 * Method 1 : `__packed` statement. It depends on compiler extension (ie, not portable).
 *            This method is safe if your compiler supports it, and *generally* as fast or faster than `memcpy`.
 * Method 2 : direct access. This method doesn't depend on compiler but violate C standard.
 *            It can generate buggy code on targets which do not support unaligned memory accesses.
 *            But in some circumstances, it's the only known way to get the most performance (ie GCC + ARMv6)
 * See http://stackoverflow.com/a/32095106/646947 for details.
 * Prefer these methods in priority order (0 > 1 > 2)
 */
#ifndef XXH_FORCE_MEMORY_ACCESS /* can be defined externally, on command line for example */
#if defined(__GNUC__) && (defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) ||           \
                          defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__) || defined(__ARM_ARCH_6T2__))
#define XXH_FORCE_MEMORY_ACCESS 2
#elif (defined(__INTEL_COMPILER) && !defined(_WIN32)) ||                                                               \
    (defined(__GNUC__) && (defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) ||          \
                           defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)))
#define XXH_FORCE_MEMORY_ACCESS 1
#endif
#endif

/*!XXH_ACCEPT_NULL_INPUT_POINTER :
 * If input pointer is NULL, xxHash default behavior is to dereference it, triggering a segfault.
 * When this macro is enabled, xxHash actively checks input for null pointer.
 * It it is, result for null input pointers is the same as a null-length input.
 */
#ifndef XXH_ACCEPT_NULL_INPUT_POINTER /* can be defined externally */
#define XXH_ACCEPT_NULL_INPUT_POINTER 0
#endif

/* *************************************
 *  Includes & Memory related functions
 ***************************************/
#include <assert.h> /* assert */
#include <limits.h> /* ULLONG_MAX for later */
#include <stdlib.h>
#include <string.h>

#define XXH_STATIC_LINKING_ONLY
#include "xxhash.h"

/* *************************************
 *  Compiler Specific Options
 ***************************************/
#ifdef _MSC_VER                 /* Visual Studio */
#pragma warning(disable : 4127) /* disable: C4127: conditional expression is constant */
#define XXH_FORCE_INLINE static __forceinline
#define XXH_NO_INLINE static __declspec(noinline)
#else
#if defined(__cplusplus) || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L /* C99 */
#ifdef __GNUC__
#define XXH_FORCE_INLINE static inline __attribute__((always_inline))
#define XXH_NO_INLINE static __attribute__((noinline))
#else
#define XXH_FORCE_INLINE static inline
#define XXH_NO_INLINE static
#endif
#else
#define XXH_FORCE_INLINE static
#define XXH_NO_INLINE static
#endif /* __STDC_VERSION__ */
#endif

/* *************************************
 *  Basic Types
 ***************************************/
#ifndef MEM_MODULE
#if !defined(__VMS) && (defined(__cplusplus) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) /* C99     \
                                                                                                             */))
#include <stdint.h>
typedef uint8_t BYTE;
typedef uint16_t U16;
typedef uint32_t U32;
#else
typedef unsigned char BYTE;
typedef unsigned short U16;
typedef unsigned int U32;
#endif
#endif

/* ===   Memory access   === */

#if (defined(XXH_FORCE_MEMORY_ACCESS) && (XXH_FORCE_MEMORY_ACCESS == 2))

/* Force direct memory access. Only works on CPU which support unaligned memory access in hardware */
static U32 XXH_read32(const void *memPtr) { return *(const U32 *)memPtr; }

#elif (defined(XXH_FORCE_MEMORY_ACCESS) && (XXH_FORCE_MEMORY_ACCESS == 1))

/* __pack instructions are safer, but compiler specific, hence potentially problematic for some compilers */
/* currently only defined for gcc and icc */
typedef union {
    U32 u32;
} __attribute__((packed)) unalign;
static U32 XXH_read32(const void *ptr) { return ((const unalign *)ptr)->u32; }

#else

/* portable and safe solution. Generally efficient.
 * see : http://stackoverflow.com/a/32095106/646947
 */
static U32 XXH_read32(const void *memPtr) {
    U32 val;
    memcpy(&val, memPtr, sizeof(val));
    return val;
}

#endif /* XXH_FORCE_DIRECT_MEMORY_ACCESS */

/* ===   Endianess   === */
typedef enum { XXH_bigEndian = 0, XXH_littleEndian = 1 } XXH_endianess;

/* XXH_CPU_LITTLE_ENDIAN can be defined externally, for example on the compiler command line */
#ifndef XXH_CPU_LITTLE_ENDIAN
static int XXH_isLittleEndian(void) {
    const union {
        U32 u;
        BYTE c[4];
    } one = {1}; /* don't use static : performance detrimental  */
    return one.c[0];
}
#define XXH_CPU_LITTLE_ENDIAN XXH_isLittleEndian()
#endif

/* ****************************************
 *  Compiler-specific Functions and Macros
 ******************************************/
#define XXH_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)

/* Note : although _rotl exists for minGW (GCC under windows), performance seems poor */
#if defined(_MSC_VER)
#define XXH_rotl32(x, r) _rotl(x, r)
#define XXH_rotl64(x, r) _rotl64(x, r)
#else
#define XXH_rotl32(x, r) (((x) << (r)) | ((x) >> (32 - (r))))
#define XXH_rotl64(x, r) (((x) << (r)) | ((x) >> (64 - (r))))
#endif

#if defined(_MSC_VER) /* Visual Studio */
#define XXH_swap32 _byteswap_ulong
#elif XXH_GCC_VERSION >= 403
#define XXH_swap32 __builtin_bswap32
#else
static U32 XXH_swap32(U32 x) {
    return ((x << 24) & 0xff000000) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | ((x >> 24) & 0x000000ff);
}
#endif

/* ***************************
 *  Memory reads
 *****************************/

U32 XXH_readLE32(const void *ptr) { return XXH_CPU_LITTLE_ENDIAN ? XXH_read32(ptr) : XXH_swap32(XXH_read32(ptr)); }
U32 XXH_readLE32_align(const void *ptr) { return XXH_readLE32(ptr); }

#ifndef XXH_NO_LONG_LONG

/* *******************************************************************
 *  64-bit hash functions
 *********************************************************************/

/*======   Memory access   ======*/

#ifndef MEM_MODULE
#define MEM_MODULE
#if !defined(__VMS) && (defined(__cplusplus) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) /* C99     \
                                                                                                             */))
#include <stdint.h>
typedef uint64_t U64;
#else
/* if compiler doesn't support unsigned long long, replace by another 64-bit type */
typedef unsigned long long U64;
#endif
#endif

#if (defined(XXH_FORCE_MEMORY_ACCESS) && (XXH_FORCE_MEMORY_ACCESS == 2))

/* Force direct memory access. Only works on CPU which support unaligned memory access in hardware */
static U64 XXH_read64(const void *memPtr) { return *(const U64 *)memPtr; }

#elif (defined(XXH_FORCE_MEMORY_ACCESS) && (XXH_FORCE_MEMORY_ACCESS == 1))

/* __pack instructions are safer, but compiler specific, hence potentially problematic for some compilers */
/* currently only defined for gcc and icc */
typedef union {
    U32 u32;
    U64 u64;
} __attribute__((packed)) unalign64;
static U64 XXH_read64(const void *ptr) { return ((const unalign64 *)ptr)->u64; }

#else

/* portable and safe solution. Generally efficient.
 * see : http://stackoverflow.com/a/32095106/646947
 */

static U64 XXH_read64(const void *memPtr) {
    U64 val;
    memcpy(&val, memPtr, sizeof(val));
    return val;
}

#endif /* XXH_FORCE_DIRECT_MEMORY_ACCESS */

#if defined(_MSC_VER) /* Visual Studio */
#define XXH_swap64 _byteswap_uint64
#elif XXH_GCC_VERSION >= 403
#define XXH_swap64 __builtin_bswap64
#else
static U64 XXH_swap64(U64 x) {
    return ((x << 56) & 0xff00000000000000ULL) | ((x << 40) & 0x00ff000000000000ULL) |
           ((x << 24) & 0x0000ff0000000000ULL) | ((x << 8) & 0x000000ff00000000ULL) |
           ((x >> 8) & 0x00000000ff000000ULL) | ((x >> 24) & 0x0000000000ff0000ULL) |
           ((x >> 40) & 0x000000000000ff00ULL) | ((x >> 56) & 0x00000000000000ffULL);
}
#endif

U64 XXH_readLE64(const void *ptr) { return XXH_CPU_LITTLE_ENDIAN ? XXH_read64(ptr) : XXH_swap64(XXH_read64(ptr)); }

/*======   xxh64   ======*/

static const U64 PRIME64_1 =
    11400714785074694791ULL; /* 0b1001111000110111011110011011000110000101111010111100101010000111 */
static const U64 PRIME64_2 =
    14029467366897019727ULL; /* 0b1100001010110010101011100011110100100111110101001110101101001111 */
static const U64 PRIME64_3 =
    1609587929392839161ULL; /* 0b0001011001010110011001111011000110011110001101110111100111111001 */
static const U64 PRIME64_4 =
    9650029242287828579ULL; /* 0b1000010111101011110010100111011111000010101100101010111001100011 */
static const U64 PRIME64_5 =
    2870177450012600261ULL; /* 0b0010011111010100111010110010111100010110010101100110011111000101 */

static U64 XXH64_round(U64 acc, U64 input) {
    acc += input * PRIME64_2;
    acc = XXH_rotl64(acc, 31);
    acc *= PRIME64_1;
    return acc;
}

static U64 XXH64_mergeRound(U64 acc, U64 val) {
    val = XXH64_round(0, val);
    acc ^= val;
    acc = acc * PRIME64_1 + PRIME64_4;
    return acc;
}

static U64 XXH64_avalanche(U64 h64) {
    h64 ^= h64 >> 33;
    h64 *= PRIME64_2;
    h64 ^= h64 >> 29;
    h64 *= PRIME64_3;
    h64 ^= h64 >> 32;
    return h64;
}

#define XXH_get32bits(p) XXH_readLE32(p)
#define XXH_get64bits(p) XXH_readLE64(p)

static U64 XXH64_finalize(U64 h64, const void *ptr, size_t len) {
    const BYTE *p = (const BYTE *)ptr;

#define PROCESS1_64                                                                                                    \
    h64 ^= (*p++) * PRIME64_5;                                                                                         \
    h64 = XXH_rotl64(h64, 11) * PRIME64_1;

#define PROCESS4_64                                                                                                    \
    h64 ^= (U64)(XXH_get32bits(p)) * PRIME64_1;                                                                        \
    p += 4;                                                                                                            \
    h64 = XXH_rotl64(h64, 23) * PRIME64_2 + PRIME64_3;

#define PROCESS8_64                                                                                                    \
    {                                                                                                                  \
        U64 const k1 = XXH64_round(0, XXH_get64bits(p));                                                               \
        p += 8;                                                                                                        \
        h64 ^= k1;                                                                                                     \
        h64 = XXH_rotl64(h64, 27) * PRIME64_1 + PRIME64_4;                                                             \
    }

    /* Rerolled version for 32-bit targets is faster and much smaller. */
    len &= 31;
    while (len >= 8) {
        PROCESS8_64;
        len -= 8;
    }
    if (len >= 4) {
        PROCESS4_64;
        len -= 4;
    }
    while (len > 0) {
        PROCESS1_64;
        --len;
    }
    return XXH64_avalanche(h64);
}

XXH_FORCE_INLINE U64 XXH64_endian_align(const void *input, size_t len, U64 seed) {
    const BYTE *p = (const BYTE *)input;
    const BYTE *bEnd = p + len;
    U64 h64;

#if defined(XXH_ACCEPT_NULL_INPUT_POINTER) && (XXH_ACCEPT_NULL_INPUT_POINTER >= 1)
    if (p == NULL) {
        len = 0;
        bEnd = p = (const BYTE *)(size_t)32;
    }
#endif

    if (len >= 32) {
        const BYTE *const limit = bEnd - 32;
        U64 v1 = seed + PRIME64_1 + PRIME64_2;
        U64 v2 = seed + PRIME64_2;
        U64 v3 = seed + 0;
        U64 v4 = seed - PRIME64_1;

        do {
            v1 = XXH64_round(v1, XXH_get64bits(p));
            p += 8;
            v2 = XXH64_round(v2, XXH_get64bits(p));
            p += 8;
            v3 = XXH64_round(v3, XXH_get64bits(p));
            p += 8;
            v4 = XXH64_round(v4, XXH_get64bits(p));
            p += 8;
        } while (p <= limit);

        h64 = XXH_rotl64(v1, 1) + XXH_rotl64(v2, 7) + XXH_rotl64(v3, 12) + XXH_rotl64(v4, 18);
        h64 = XXH64_mergeRound(h64, v1);
        h64 = XXH64_mergeRound(h64, v2);
        h64 = XXH64_mergeRound(h64, v3);
        h64 = XXH64_mergeRound(h64, v4);

    } else {
        h64 = seed + PRIME64_5;
    }

    h64 += (U64)len;

    return XXH64_finalize(h64, p, len);
}

XXH_PUBLIC_API XXH64_hash_t XXH64(const void *input, size_t len, unsigned long long seed) {
    return XXH64_endian_align(input, len, seed);
}

#endif /* XXH_NO_LONG_LONG */
