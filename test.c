#include <assert.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <stddef.h>


typedef uint8_t uint8x16 __attribute((vector_size(16)));
typedef int16_t int16x8 __attribute((vector_size(16)));
typedef uint64_t uint64x2 __attribute((vector_size(16)));
typedef uint32_t uint32x4 __attribute((vector_size(16)));


static inline int16x8 splat_vec(int16_t val) {
    return (int16x8){val, val, val, val, val, val, val, val};
}

static inline int16x8 select_vec(const int16x8 cond, const int16x8 a, const int16x8 b) {
    return (int16x8)__builtin_wasm_bitselect(a, b, cond);
}

static inline int16x8 expand_pixels(const uint8x16 pixels) {
    const uint8x16 zero = (uint8x16){0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0};
    return (int16x8)__builtin_shufflevector(pixels, zero,
        0, 16,
        1, 16,
        2, 16,
        3, 16,
        4, 16,
        5, 16,
        6, 16,
        7, 16
    );
}

static inline uint8x16 merge_pixels(const int16x8 work, const uint8x16 orig) {
    return (uint8x16)__builtin_shufflevector((uint8x16)work, orig,
         0,  2,  4,  6,   8, 10, 12, 14, // the 8 pixels we worked on
        24, 25, 26, 27,  28, 29, 30, 31 // the next 8 pixels, not yet processed
    );
}

__attribute__((noinline))
static int16x8 do_shift(int16x8 px_v)
{
    return (px_v - 1) >> 4;
}

__attribute__((noinline))
static void dostuff(uint8_t *dst, ptrdiff_t stride)
{
    const int x = 0;

    const uint8x16 px_raw = *(const uint8x16 *)(&dst[x]);
    int16x8 px_v = expand_pixels(px_raw);

    px_v = do_shift(px_v);

    const uint64_t dst_64 = ((uint64x2)merge_pixels(px_v, px_raw))[0];
    *(uint64_t *)(&dst[0]) = dst_64;
}

int main(int argc, const char **argv) {
    uint8_t *dst = malloc(65536);

    const int stride = 1024;
    dst[0] = 0x00;
    dst[1] = 0x11;
    dst[2] = 0x22;
    dst[3] = 0x33;
    dst[4] = 0x44;
    dst[5] = 0x55;
    dst[6] = 0x66;
    dst[7] = 0x77;

    dostuff(dst, stride);

    printf("%d %d %d %d  %d %d %d %d\n",
        dst[0], dst[1], dst[2], dst[3],
        dst[4], dst[5], dst[6], dst[7]);

    return 0;
}
