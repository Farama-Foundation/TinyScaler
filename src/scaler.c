#include "scaler.h"

// Nearest does not use SIMD
void scale_nearest_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height) {
    f32 ratio_x = (f32)src_width / (f32)dst_width;
    f32 ratio_y = (f32)src_height / (f32)dst_height;
    i32 dst_width4 = dst_width << 2;
    i32 src_width4 = src_width << 2;

    for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
        i32 src_y = (i32)((dst_y + 0.5f) * ratio_y);

        i32 dst_offset4 = dst_width4 * dst_y;
        i32 src_offset4 = src_width4 * src_y;

        for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
            i32 src_x = (i32)((dst_x + 0.5f) * ratio_x);

            memcpy(&dst[(dst_x << 2) + dst_offset4], &src[(src_x << 2) + src_offset4], RGBA32F_SIZE);
        }
    }
}

#if defined(ENABLE_SIMD)

#if defined(__x86_64__) // SSE implementation

void scale_bilinear_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height) {
    f32 ratio_x = (f32)(src_width - 1) / (f32)dst_width;
    f32 ratio_y = (f32)(src_height - 1) / (f32)dst_height;
    i32 dst_width4 = dst_width << 2;
    i32 src_width4 = src_width << 2;
    i32 src_width4_4 = src_width4 + 4;

    if ((((size_t)src | (size_t)dst) & 0x0f) == 0) { // Aligned memory
        for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
            f32 src_y_f = (dst_y + 0.5f) * ratio_y;
            i32 src_y = (i32)src_y_f;
            f32 interp_y = src_y_f - src_y;

            __m128 iy = _mm_set1_ps(interp_y);
            __m128 iy1 = _mm_set1_ps(1.0f - interp_y);

            i32 dst_offset4 = dst_width4 * dst_y;
            i32 src_offset4 = src_width4 * src_y;

            for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
                f32 src_x_f = (dst_x + 0.5f) * ratio_x;
                i32 src_x = (i32)src_x_f;
                f32 interp_x = src_x_f - src_x;

                i32 dst_start = (dst_x << 2) + dst_offset4;

                f32* src_start00 = src + (src_x << 2) + src_offset4;

                __m128 ix = _mm_set1_ps(interp_x);
                __m128 ix1 = _mm_set1_ps(1.0f - interp_x);

                __m128 p00 = _mm_load_ps(src_start00);
                __m128 p01 = _mm_load_ps(src_start00 + 4);
                __m128 p10 = _mm_load_ps(src_start00 + src_width4);
                __m128 p11 = _mm_load_ps(src_start00 + src_width4_4);

                p00 = _mm_add_ps(_mm_mul_ps(p00, iy1), _mm_mul_ps(p10, iy));
                p01 = _mm_add_ps(_mm_mul_ps(p01, iy1), _mm_mul_ps(p11, iy));

                p00 = _mm_add_ps(_mm_mul_ps(p00, ix1), _mm_mul_ps(p01, ix));

                _mm_store_ps(dst + dst_start, p00);
            }
        }
    }
    else { // Unaligned memory
        for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
            f32 src_y_f = (dst_y + 0.5f) * ratio_y;
            i32 src_y = (i32)src_y_f;
            f32 interp_y = src_y_f - src_y;

            __m128 iy = _mm_set1_ps(interp_y);
            __m128 iy1 = _mm_set1_ps(1.0f - interp_y);

            i32 dst_offset4 = dst_width4 * dst_y;
            i32 src_offset4 = src_width4 * src_y;

            for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
                f32 src_x_f = (dst_x + 0.5f) * ratio_x;
                i32 src_x = (i32)src_x_f;
                f32 interp_x = src_x_f - src_x;

                i32 dst_start = (dst_x << 2) + dst_offset4;

                f32* src_start00 = src + (src_x << 2) + src_offset4;

                __m128 ix = _mm_set1_ps(interp_x);
                __m128 ix1 = _mm_set1_ps(1.0f - interp_x);

                __m128 p00 = _mm_loadu_ps(src_start00);
                __m128 p01 = _mm_loadu_ps(src_start00 + 4);
                __m128 p10 = _mm_loadu_ps(src_start00 + src_width4);
                __m128 p11 = _mm_loadu_ps(src_start00 + src_width4_4);

                p00 = _mm_add_ps(_mm_mul_ps(p00, iy1), _mm_mul_ps(p10, iy));
                p01 = _mm_add_ps(_mm_mul_ps(p01, iy1), _mm_mul_ps(p11, iy));

                p00 = _mm_add_ps(_mm_mul_ps(p00, ix1), _mm_mul_ps(p01, ix));

                _mm_storeu_ps(dst + dst_start, p00);
            }
        }
    }
}

void scale_area_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height) {
    f32 ratio_x = (f32)src_width / (f32)dst_width;
    f32 ratio_y = (f32)src_height / (f32)dst_height;
    i32 src_width4 = src_width << 2;
    i32 dst_width4 = dst_width << 2;

    if ((((size_t)src | (size_t)dst) & 0x0f) == 0) { // Aligned memory
        for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
            f32 src_lower_y_f = dst_y * ratio_y;
            f32 src_upper_y_f = (dst_y + 1.0f) * ratio_y;

            i32 src_lower_y = max(0, (i32)src_lower_y_f);
            i32 src_upper_y = min(src_height, (i32)src_upper_y_f);

            f32 over_height = src_lower_y_f - (i32)src_lower_y_f;
            f32 over_height1 = 1.0f - (src_upper_y_f - (i32)src_lower_y_f);

            i32 dst_offset4 = dst_width4 * dst_y;

            for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
                f32 src_lower_x_f = dst_x * ratio_x;
                f32 src_upper_x_f = (dst_x + 1.0f) * ratio_x;

                i32 src_lower_x = max(0, (i32)src_lower_x_f);
                i32 src_upper_x = min(src_width, (i32)src_upper_x_f + 1);

                f32 over_width = src_lower_x_f - (i32)src_lower_x_f;
                f32 over_width1 = 1.0f - (src_upper_x_f - (i32)src_lower_x_f);

                i32 dst_start = (dst_x << 2) + dst_offset4;

                __m128 res = _mm_set1_ps(0.0f);

                f32 weight_total = 0.0f;

                for (i32 area_y = src_lower_y; area_y < src_upper_y; area_y++) {
                    i32 src_offset4 = src_width4 * area_y;

                    for (i32 area_x = src_lower_x; area_x < src_upper_x; area_x++) {
                        f32 weight = (1.0f - (area_y == src_lower_y) * over_height) * (1.0f - (area_x == src_lower_x) * over_width) +
                            (1.0f - (area_y == src_upper_y) * over_height1) * (1.0f - (area_x == src_upper_x) * over_width1);

                        i32 src_start = (area_x << 2) + src_offset4;

                        __m128 p = _mm_load_ps(src + src_start);

                        res = _mm_add_ps(res, _mm_mul_ps(p, _mm_set1_ps(weight)));
                        weight_total += weight;
                    }
                }

                f32 div = 1.0f / weight_total;

                res = _mm_mul_ps(res, _mm_set1_ps(div));

                _mm_store_ps(dst + dst_start, res);
            }
        }
    }
    else { // Unaligned memory
        for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
            f32 src_lower_y_f = dst_y * ratio_y;
            f32 src_upper_y_f = (dst_y + 1.0f) * ratio_y;

            i32 src_lower_y = max(0, (i32)src_lower_y_f);
            i32 src_upper_y = min(src_height, (i32)src_upper_y_f);

            f32 over_height = src_lower_y_f - (i32)src_lower_y_f;
            f32 over_height1 = 1.0f - (src_upper_y_f - (i32)src_lower_y_f);

            i32 dst_offset4 = dst_width4 * dst_y;

            for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
                f32 src_lower_x_f = dst_x * ratio_x;
                f32 src_upper_x_f = (dst_x + 1.0f) * ratio_x;

                i32 src_lower_x = max(0, (i32)src_lower_x_f);
                i32 src_upper_x = min(src_width, (i32)src_upper_x_f + 1);

                f32 over_width = src_lower_x_f - (i32)src_lower_x_f;
                f32 over_width1 = 1.0f - (src_upper_x_f - (i32)src_lower_x_f);

                i32 dst_start = (dst_x << 2) + dst_offset4;

                __m128 res = _mm_set1_ps(0.0f);

                f32 weight_total = 0.0f;

                for (i32 area_y = src_lower_y; area_y < src_upper_y; area_y++) {
                    i32 src_offset4 = src_width4 * area_y;

                    for (i32 area_x = src_lower_x; area_x < src_upper_x; area_x++) {
                        f32 weight = (1.0f - (area_y == src_lower_y) * over_height) * (1.0f - (area_x == src_lower_x) * over_width) +
                            (1.0f - (area_y == src_upper_y) * over_height1) * (1.0f - (area_x == src_upper_x) * over_width1);

                        i32 src_start = (area_x << 2) + src_offset4;

                        __m128 p = _mm_loadu_ps(src + src_start);

                        res = _mm_add_ps(res, _mm_mul_ps(p, _mm_set1_ps(weight)));
                        weight_total += weight;
                    }
                }

                f32 div = 1.0f / weight_total;

                res = _mm_mul_ps(res, _mm_set1_ps(div));

                _mm_storeu_ps(dst + dst_start, res);
            }
        }
    }
}

#elif defined(__arm__) // ARM Neon implementation

void scale_bilinear_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height) {
    f32 ratio_x = (f32)(src_width - 1) / (f32)dst_width;
    f32 ratio_y = (f32)(src_height - 1) / (f32)dst_height;
    i32 dst_width4 = dst_width << 2;
    i32 src_width4 = src_width << 2;
    i32 src_width4_4 = src_width4 + 4;

    for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
        f32 src_y_f = (dst_y + 0.5f) * ratio_y;
        i32 src_y = (i32)src_y_f;
        f32 interp_y = src_y_f - src_y;

        float32x4_t iy = vdupq_n_f32(interp_y);
        float32x4_t iy1 = vdupq_n_f32(1.0f - interp_y);

        i32 dst_offset4 = dst_width4 * dst_y;
        i32 src_offset4 = src_width4 * src_y;

        for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
            f32 src_x_f = (dst_x + 0.5f) * ratio_x;
            i32 src_x = (i32)src_x_f;
            f32 interp_x = src_x_f - src_x;

            i32 dst_start = (dst_x << 2) + dst_offset4;

            f32* src_start00 = src + (src_x << 2) + src_offset4;

            float32x4_t ix = vdupq_n_f32(interp_x); // Use q versions (128 bit)
            float32x4_t ix1 = vdupq_n_f32(1.0f - interp_x);

            float32x4_t p00 = vld1q_f32(src_start00);
            float32x4_t p01 = vld1q_f32(src_start00 + 4);
            float32x4_t p10 = vld1q_f32(src_start00 + src_width4);
            float32x4_t p11 = vld1q_f32(src_start00 + src_width4_4);

            p00 = vaddq_f32(vmulq_f32(p00, iy1), vmulq_f32(p10, iy));
            p01 = vaddq_f32(vmulq_f32(p01, iy1), vmulq_f32(p11, iy));

            p00 = vaddq_f32(vmulq_f32(p00, ix1), vmulq_f32(p01, ix));

            vst1q_f32(dst + dst_start, p00);
        }
    }
}

void scale_area_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height) {
    f32 ratio_x = (f32)src_width / (f32)dst_width;
    f32 ratio_y = (f32)src_height / (f32)dst_height;
    i32 src_width4 = src_width << 2;
    i32 dst_width4 = dst_width << 2;

    for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
        f32 src_lower_y_f = dst_y * ratio_y;
        f32 src_upper_y_f = (dst_y + 1.0f) * ratio_y;

        i32 src_lower_y = max(0, (i32)src_lower_y_f);
        i32 src_upper_y = min(src_height, (i32)src_upper_y_f);

        f32 over_height = src_lower_y_f - (i32)src_lower_y_f;
        f32 over_height1 = 1.0f - (src_upper_y_f - (i32)src_lower_y_f);

        i32 dst_offset4 = dst_width4 * dst_y;

        for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
            f32 src_lower_x_f = dst_x * ratio_x;
            f32 src_upper_x_f = (dst_x + 1.0f) * ratio_x;

            i32 src_lower_x = max(0, (i32)src_lower_x_f);
            i32 src_upper_x = min(src_width, (i32)src_upper_x_f + 1);

            f32 over_width = src_lower_x_f - (i32)src_lower_x_f;
            f32 over_width1 = 1.0f - (src_upper_x_f - (i32)src_lower_x_f);

            i32 dst_start = (dst_x << 2) + dst_offset4;

            float32x4_t res = vdupq_n_f32(0.0f);

            f32 weight_total = 0.0f;

            for (i32 area_y = src_lower_y; area_y < src_upper_y; area_y++) {
                i32 src_offset4 = src_width4 * area_y;

                for (i32 area_x = src_lower_x; area_x < src_upper_x; area_x++) {
                    f32 weight = (1.0f - (area_y == src_lower_y) * over_height) * (1.0f - (area_x == src_lower_x) * over_width) +
                        (1.0f - (area_y == src_upper_y) * over_height1) * (1.0f - (area_x == src_upper_x) * over_width1);

                    i32 src_start = (area_x << 2) + src_offset4;

                    float32x4_t p = vld1q_f32(src + src_start);

                    res = vaddq_f32(res, vmulq_f32(p, vdupq_n_f32(weight)));
                    weight_total += weight;
                }
            }

            f32 div = 1.0f / weight_total;

            res = vmulq_f32(res, vdupq_n_f32(div));

            vst1q_f32(dst + dst_start, res);
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
    i32 dst_width4 = dst_width << 2;

    i32 src_width4s[8];
    src_width4s[0] = src_width << 2;
    
    for (i32 i = 1; i < 8; i++)
        src_width4s[i] = src_width4s[0] + i;

    for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
        f32 src_y_f = (dst_y + 0.5f) * ratio_y;
        i32 src_y = (i32)src_y_f;
        f32 interp_y = src_y_f - src_y;
        f32 interp_y1 = 1.0f - interp_y;

        i32 dst_offset4 = dst_width4 * dst_y;
        i32 src_offset4 = src_width4s[0] * src_y;

        for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
            f32 src_x_f = (dst_x + 0.5f) * ratio_x;
            i32 src_x = (i32)src_x_f;
            f32 interp_x = src_x_f - src_x;

            i32 dst_start = (dst_x << 2) + dst_offset4;

            i32 src_start00 = (src_x << 2) + src_offset4;

            f32 interp_x1 = 1.0f - interp_x;

            f32 pr0 = interp_y1 * src[src_start00    ] + interp_y * src[src_start00 + src_width4s[0]];
            f32 pr1 = interp_y1 * src[src_start00 + 4] + interp_y * src[src_start00 + src_width4s[4]];

            f32 pg0 = interp_y1 * src[src_start00 + 1] + interp_y * src[src_start00 + src_width4s[1]];
            f32 pg1 = interp_y1 * src[src_start00 + 5] + interp_y * src[src_start00 + src_width4s[5]];

            f32 pb0 = interp_y1 * src[src_start00 + 2] + interp_y * src[src_start00 + src_width4s[2]];
            f32 pb1 = interp_y1 * src[src_start00 + 6] + interp_y * src[src_start00 + src_width4s[6]];

            f32 pa0 = interp_y1 * src[src_start00 + 3] + interp_y * src[src_start00 + src_width4s[3]];
            f32 pa1 = interp_y1 * src[src_start00 + 7] + interp_y * src[src_start00 + src_width4s[7]];

            dst[dst_start    ] = interp_x1 * pr0 + interp_x * pr1;
            dst[dst_start + 1] = interp_x1 * pg0 + interp_x * pg1;
            dst[dst_start + 2] = interp_x1 * pb0 + interp_x * pb1;
            dst[dst_start + 3] = interp_x1 * pa0 + interp_x * pa1;
        }
    }
}

void scale_area_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height) {
    f32 ratio_x = (f32)src_width / (f32)dst_width;
    f32 ratio_y = (f32)src_height / (f32)dst_height;
    i32 src_width4 = src_width << 2;
    i32 dst_width4 = dst_width << 2;

    for (i32 dst_y = 0; dst_y < dst_height; dst_y++) {
        f32 src_lower_y_f = dst_y * ratio_y;
        f32 src_upper_y_f = (dst_y + 1.0f) * ratio_y;

        i32 src_lower_y = max(0, (i32)src_lower_y_f);
        i32 src_upper_y = min(src_height, (i32)src_upper_y_f);

        f32 over_height = src_lower_y_f - (i32)src_lower_y_f;
        f32 over_height1 = 1.0f - (src_upper_y_f - (i32)src_lower_y_f);

        i32 dst_offset4 = dst_width4 * dst_y;

        for (i32 dst_x = 0; dst_x < dst_width; dst_x++) {
            f32 src_lower_x_f = dst_x * ratio_x;
            f32 src_upper_x_f = (dst_x + 1.0f) * ratio_x;

            i32 src_lower_x = max(0, (i32)src_lower_x_f);
            i32 src_upper_x = min(src_width, (i32)src_upper_x_f + 1);

            f32 over_width = src_lower_x_f - (i32)src_lower_x_f;
            f32 over_width1 = 1.0f - (src_upper_x_f - (i32)src_lower_x_f);

            i32 dst_start = (dst_x << 2) + dst_offset4;

            f32 r = 0.0f;
            f32 g = 0.0f;
            f32 b = 0.0f;
            f32 a = 0.0f;

            f32 weight_total = 0.0f;

            for (i32 area_y = src_lower_y; area_y < src_upper_y; area_y++) {
                i32 src_offset4 = src_width4 * area_y;

                for (i32 area_x = src_lower_x; area_x < src_upper_x; area_x++) {
                    f32 weight = (1.0f - (area_y == src_lower_y) * over_height) * (1.0f - (area_x == src_lower_x) * over_width) +
                        (1.0f - (area_y == src_upper_y) * over_height1) * (1.0f - (area_x == src_upper_x) * over_width1);

                    i32 src_start = (area_x << 2) + src_offset4;

                    r += weight * src[src_start    ];
                    g += weight * src[src_start + 1];
                    b += weight * src[src_start + 2];
                    a += weight * src[src_start + 3];
                    weight_total += weight;
                }
            }

            f32 div = 1.0f / weight_total;

            dst[dst_start    ] = r * div;
            dst[dst_start + 1] = g * div;
            dst[dst_start + 2] = b * div;
            dst[dst_start + 3] = a * div;
        }
    }
}

#endif
