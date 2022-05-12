#pragma once

#include <stdlib.h>
#include <math.h>
#include <memory.h>

#if defined(__x86_64__)
#include <pmmintrin.h> // SSE
#elif defined(__arm__)
#include <arm_neon.h> // Neon
#endif

#define RGBA32F_SIZE 16 // Byte size of a pixel

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

typedef unsigned char u8;
typedef int i32;
typedef float f32;
typedef double f64;

// Scalers
void scale_nearest_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height);
void scale_bilinear_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height);
void scale_area_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height);
