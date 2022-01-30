#include "scaler.h"

// Nearest does not use SIMD
void scale_nearest_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height) {
    f32 ratio_x = (f32)src_width / (f32)dst_width;
    f32 ratio_y = (f32)src_height / (f32)dst_height;

    i32 dst_size = dst_width * dst_height;

    for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
        for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
            i32 src_x = (i32)((dst_x + 0.5f) * ratio_x);
            i32 src_y = (i32)((dst_y + 0.5f) * ratio_y);

            memcpy(&dst[4 * (dst_y + dst_height * dst_x)], &src[4 * (src_y + src_height * src_x)], RGBA32F_SIZE);
        }
    }
}

#if defined(ENABLE_SIMD)

#if defined(__x86_64__) // SSE implementation

void scale_bilinear_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height) {
    f32 ratio_x = (f32)(src_width - 1) / (f32)dst_width;
    f32 ratio_y = (f32)(src_height - 1) / (f32)dst_height;

    for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
        for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
            f32 src_x_f = (dst_x + 0.5f) * ratio_x;
            f32 src_y_f = (dst_y + 0.5f) * ratio_y;
            i32 src_x = (i32)src_x_f;
            i32 src_y = (i32)src_y_f;
            f32 interp_x = src_x_f - src_x;
            f32 interp_y = src_y_f - src_y;

            i32 dst_start = 4 * (dst_y + dst_height * dst_x);

            i32 src_start00 = 4 * (src_y + src_height * src_x);
            i32 src_start01 = src_start00 + 4;
            i32 src_start10 = src_start00 + src_height * 4;
            i32 src_start11 = src_start10 + 4;

            __m128 ix = _mm_set1_ps(interp_x);
            __m128 ix1 = _mm_set1_ps(1.0f - interp_x);
            __m128 iy = _mm_set1_ps(interp_y);
            __m128 iy1 = _mm_set1_ps(1.0f - interp_y);

            __m128 p00 = _mm_load_ps(src + src_start00);
            __m128 p01 = _mm_load_ps(src + src_start01);
            __m128 p10 = _mm_load_ps(src + src_start10);
            __m128 p11 = _mm_load_ps(src + src_start11);

            p00 = _mm_add_ps(_mm_mul_ps(p00, ix1), _mm_mul_ps(p10, ix));
            p01 = _mm_add_ps(_mm_mul_ps(p01, ix1), _mm_mul_ps(p11, ix));

            p00 = _mm_add_ps(_mm_mul_ps(p00, iy1), _mm_mul_ps(p01, iy));

            _mm_store_ps(dst + dst_start, p00);
        }
    }
}

#elif defined(__ARM_NEON) // ARM Neon implementation

void scale_bilinear_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height) {
    f32 ratio_x = (f32)(src_width - 1) / (f32)dst_width;
    f32 ratio_y = (f32)(src_height - 1) / (f32)dst_height;

    for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
        for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
            f32 src_x_f = (dst_x + 0.5f) * ratio_x;
            f32 src_y_f = (dst_y + 0.5f) * ratio_y;
            i32 src_x = (i32)src_x_f;
            i32 src_y = (i32)src_y_f;
            f32 interp_x = src_x_f - src_x;
            f32 interp_y = src_y_f - src_y;

            i32 dst_start = 4 * (dst_y + dst_height * dst_x);

            i32 src_start00 = 4 * (src_y + src_height * src_x);
            i32 src_start01 = src_start00 + 4;
            i32 src_start10 = src_start00 + src_height * 4;
            i32 src_start11 = src_start10 + 4;

            float32x4_t ix = vdupq_n_f32(interp_x);
            float32x4_t ix1 = vdupq_n_f32(1.0f - interp_x);
            float32x4_t iy = vdupq_n_f32(interp_y);
            float32x4_t iy1 = vdupq_n_f32(1.0f - interp_y);

            float32x4_t p00 = vld1q_f32(src + src_start00);
            float32x4_t p01 = vld1q_f32(src + src_start01);
            float32x4_t p10 = vld1q_f32(src + src_start10);
            float32x4_t p11 = vld1q_f32(src + src_start11);

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

    for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
        for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
            f32 src_x_f = (dst_x + 0.5f) * ratio_x;
            f32 src_y_f = (dst_y + 0.5f) * ratio_y;
            i32 src_x = (i32)src_x_f;
            i32 src_y = (i32)src_y_f;
            f32 interp_x = src_x_f - src_x;
            f32 interp_y = src_y_f - src_y;

            i32 dst_start = 4 * (dst_y + dst_height * dst_x);

            i32 src_start00 = 4 * (src_y + src_height * src_x);
            i32 src_start01 = src_start00 + 4;
            i32 src_start10 = src_start00 + src_height * 4;
            i32 src_start11 = src_start10 + 4;

            f32 interp_x1 = 1.0f - interp_x;
            f32 interp_y1 = 1.0f - interp_y;

            f32 pr0 = interp_x1 * src[src_start00    ] + interp_x * src[src_start10    ];
            f32 pr1 = interp_x1 * src[src_start01    ] + interp_x * src[src_start11    ];

            f32 pg0 = interp_x1 * src[src_start00 + 1] + interp_x * src[src_start10 + 1];
            f32 pg1 = interp_x1 * src[src_start01 + 1] + interp_x * src[src_start11 + 1];

            f32 pb0 = interp_x1 * src[src_start00 + 2] + interp_x * src[src_start10 + 2];
            f32 pb1 = interp_x1 * src[src_start01 + 2] + interp_x * src[src_start11 + 2];

            f32 pa0 = interp_x1 * src[src_start00 + 3] + interp_x * src[src_start10 + 3];
            f32 pa1 = interp_x1 * src[src_start01 + 3] + interp_x * src[src_start11 + 3];

            dst[dst_start    ] = interp_y1 * pr0 + interp_y * pr1;
            dst[dst_start + 1] = interp_y1 * pg0 + interp_y * pg1;
            dst[dst_start + 2] = interp_y1 * pb0 + interp_y * pb1;
            dst[dst_start + 3] = interp_y1 * pa0 + interp_y * pa1;
        }
    }
}

#endif
