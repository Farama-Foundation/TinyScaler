#include "scaler.h"

// Nearest does not use SIMD
void scale_nearest_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height) {
    f32 ratio_x = (f32)src_width / (f32)dst_width;
    f32 ratio_y = (f32)src_height / (f32)dst_height;
    i32 dst_height4 = dst_height << 2;
    i32 src_height4 = src_height << 2;

    for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
        i32 src_x = (i32)((dst_x + 0.5f) * ratio_x);

        i32 dst_offset4 = dst_height4 * dst_x;
        i32 src_offset4 = src_height4 * src_x;

        for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
            i32 src_y = (i32)((dst_y + 0.5f) * ratio_y);

            memcpy(&dst[(dst_y << 2) + dst_offset4], &src[(src_y << 2) + src_offset4], RGBA32F_SIZE);
        }
    }
}

#if defined(ENABLE_SIMD)

#if defined(__x86_64__) // SSE implementation

void scale_bilinear_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height) {
    f32 ratio_x = (f32)(src_width - 1) / (f32)dst_width;
    f32 ratio_y = (f32)(src_height - 1) / (f32)dst_height;
    i32 dst_height4 = dst_height << 2;
    i32 src_height4 = src_height << 2;
    i32 src_height4_4 = src_height4 + 4;

    if ((((size_t)src | (size_t)dst) & 0x0f) == 0) { // Aligned memory
        for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
            f32 src_x_f = (dst_x + 0.5f) * ratio_x;
            i32 src_x = (i32)src_x_f;
            f32 interp_x = src_x_f - src_x;

            __m128 ix = _mm_set1_ps(interp_x);
            __m128 ix1 = _mm_set1_ps(1.0f - interp_x);

            i32 dst_offset4 = dst_height4 * dst_x;
            i32 src_offset4 = src_height4 * src_x;

            for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
                f32 src_y_f = (dst_y + 0.5f) * ratio_y;
                i32 src_y = (i32)src_y_f;
                f32 interp_y = src_y_f - src_y;

                i32 dst_start = (dst_y << 2) + dst_offset4;

                f32* src_start00 = src + (src_y << 2) + src_offset4;

                __m128 iy = _mm_set1_ps(interp_y);
                __m128 iy1 = _mm_set1_ps(1.0f - interp_y);

                __m128 p00 = _mm_load_ps(src_start00);
                __m128 p01 = _mm_load_ps(src_start00 + 4);
                __m128 p10 = _mm_load_ps(src_start00 + src_height4);
                __m128 p11 = _mm_load_ps(src_start00 + src_height4_4);

                p00 = _mm_add_ps(_mm_mul_ps(p00, ix1), _mm_mul_ps(p10, ix));
                p01 = _mm_add_ps(_mm_mul_ps(p01, ix1), _mm_mul_ps(p11, ix));

                p00 = _mm_add_ps(_mm_mul_ps(p00, iy1), _mm_mul_ps(p01, iy));

                _mm_store_ps(dst + dst_start, p00);
            }
        }
    }
    else { // Unaligned memory
        for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
            f32 src_x_f = (dst_x + 0.5f) * ratio_x;
            i32 src_x = (i32)src_x_f;
            f32 interp_x = src_x_f - src_x;

            __m128 ix = _mm_set1_ps(interp_x);
            __m128 ix1 = _mm_set1_ps(1.0f - interp_x);

            i32 dst_offset4 = dst_height4 * dst_x;
            i32 src_offset4 = src_height4 * src_x;

            for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
                f32 src_y_f = (dst_y + 0.5f) * ratio_y;
                i32 src_y = (i32)src_y_f;
                f32 interp_y = src_y_f - src_y;

                i32 dst_start = (dst_y << 2) + dst_offset4;

                f32* src_start00 = src + (src_y << 2) + src_offset4;

                __m128 iy = _mm_set1_ps(interp_y);
                __m128 iy1 = _mm_set1_ps(1.0f - interp_y);

                __m128 p00 = _mm_loadu_ps(src_start00);
                __m128 p01 = _mm_loadu_ps(src_start00 + 4);
                __m128 p10 = _mm_loadu_ps(src_start00 + src_height4);
                __m128 p11 = _mm_loadu_ps(src_start00 + src_height4_4);

                p00 = _mm_add_ps(_mm_mul_ps(p00, ix1), _mm_mul_ps(p10, ix));
                p01 = _mm_add_ps(_mm_mul_ps(p01, ix1), _mm_mul_ps(p11, ix));

                p00 = _mm_add_ps(_mm_mul_ps(p00, iy1), _mm_mul_ps(p01, iy));

                _mm_storeu_ps(dst + dst_start, p00);
            }
        }
    }
}

#elif defined(__arm__) // ARM Neon implementation

void scale_bilinear_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height) {
    f32 ratio_x = (f32)(src_width - 1) / (f32)dst_width;
    f32 ratio_y = (f32)(src_height - 1) / (f32)dst_height;
    i32 dst_height4 = dst_height << 2;
    i32 src_height4 = src_height << 2;
    i32 src_height4_4 = src_height4 + 4;

    for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
        f32 src_x_f = (dst_x + 0.5f) * ratio_x;
        i32 src_x = (i32)src_x_f;
        f32 interp_x = src_x_f - src_x;

        float32x4_t ix = vdupq_n_f32(interp_x);
        float32x4_t ix1 = vdupq_n_f32(1.0f - interp_x);

        i32 dst_offset4 = dst_height4 * dst_x;
        i32 src_offset4 = src_height4 * src_x;

        for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
            f32 src_y_f = (dst_y + 0.5f) * ratio_y;
            i32 src_y = (i32)src_y_f;
            f32 interp_y = src_y_f - src_y;

            i32 dst_start = (dst_y << 2) + dst_offset4;

            float32x4_t iy = vdupq_n_f32(interp_y); // Use q versions (128 bit)
            float32x4_t iy1 = vdupq_n_f32(1.0f - interp_y);

            f32* src_start00 = src + (src_y << 2) + src_offset4;

            float32x4_t p00 = vld1q_f32(src_start00);
            float32x4_t p01 = vld1q_f32(src_start00 + 4);
            float32x4_t p10 = vld1q_f32(src_start00 + src_height4);
            float32x4_t p11 = vld1q_f32(src_start00 + src_height4_4);

            p00 = vaddq_f32(vmulq_f32(p00, ix1), vmulq_f32(p10, ix));
            p01 = vaddq_f32(vmulq_f32(p01, ix1), vmulq_f32(p11, ix));

            p00 = vaddq_f32(vmulq_f32(p00, iy1), vmulq_f32(p01, iy));

            vst1q_f32(dst + dst_start, p00);
        }
    }
}

#else
#error *** SIMD Enabled but no support found! ***
#endif

#else // No SIMD implementation

void scale_bilinear_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height) {
    f32 ratio_x = (f32)(src_width - 1) / (f32)dst_width;
    f32 ratio_y = (f32)(src_height - 1) / (f32)dst_height;
    i32 dst_height4 = dst_height << 2;

    i32 src_height4s[8];
    src_height4s[0] = src_height << 2;
    
    for (i32 i = 1; i < 8; i++)
        src_height4s[i] = src_height4s[0] + i;

    for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
        f32 src_x_f = (dst_x + 0.5f) * ratio_x;
        i32 src_x = (i32)src_x_f;
        f32 interp_x = src_x_f - src_x;
        f32 interp_x1 = 1.0f - interp_x;

        i32 dst_offset4 = dst_height4 * dst_x;
        i32 src_offset4 = src_height4s[0] * src_x;

        for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
            f32 src_y_f = (dst_y + 0.5f) * ratio_y;
            i32 src_y = (i32)src_y_f;
            f32 interp_y = src_y_f - src_y;

            i32 dst_start = (dst_y << 2) + dst_offset4;

            i32 src_start00 = (src_y << 2) + src_offset4;

            f32 interp_y1 = 1.0f - interp_y;

            f32 pr0 = interp_x1 * src[src_start00    ] + interp_x * src[src_start00 + src_height4s[0]];
            f32 pr1 = interp_x1 * src[src_start00 + 4] + interp_x * src[src_start00 + src_height4s[4]];

            f32 pg0 = interp_x1 * src[src_start00 + 1] + interp_x * src[src_start00 + src_height4s[1]];
            f32 pg1 = interp_x1 * src[src_start00 + 5] + interp_x * src[src_start00 + src_height4s[5]];

            f32 pb0 = interp_x1 * src[src_start00 + 2] + interp_x * src[src_start00 + src_height4s[2]];
            f32 pb1 = interp_x1 * src[src_start00 + 6] + interp_x * src[src_start00 + src_height4s[6]];

            f32 pa0 = interp_x1 * src[src_start00 + 3] + interp_x * src[src_start00 + src_height4s[3]];
            f32 pa1 = interp_x1 * src[src_start00 + 7] + interp_x * src[src_start00 + src_height4s[7]];

            dst[dst_start    ] = interp_y1 * pr0 + interp_y * pr1;
            dst[dst_start + 1] = interp_y1 * pg0 + interp_y * pg1;
            dst[dst_start + 2] = interp_y1 * pb0 + interp_y * pb1;
            dst[dst_start + 3] = interp_y1 * pa0 + interp_y * pa1;
        }
    }
}

#endif
