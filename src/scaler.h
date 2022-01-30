#pragma once

#include <math.h>
#include <memory.h>

#if defined(ENABLE_SIMD)

// Must define __ARM_NEON manually for some platforms
#if defined(ENABLE_NEON) && !defined(__ARM_NEON)
#define __ARM_NEON 1
#endif

// For backwards compatibility on Arm
#if defined (__ARM_NEON__) && !defined(__ARM_NEON)
#define __ARM_NEON 1
#endif

#if defined(__x86_64__)
#include <pmmintrin.h> // SSE
#elif defined(__ARM_NEON)
#include <arm_neon.h> // Neon
#endif

#endif

#define RGBA32F_SIZE 16 // Byte size of a pixel

typedef unsigned char u8;
typedef int i32;
typedef float f32;
typedef double f64;

// Scalers
void scale_nearest_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height);
void scale_bilinear_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height);
