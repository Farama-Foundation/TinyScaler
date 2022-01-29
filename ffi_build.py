from cffi import FFI

ffibuilder = FFI()

ffibuilder.cdef("""
    typedef unsigned char u8;
    typedef int i32;
    typedef float f32;
    typedef double f64;

    void scale_nearest_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height);
    void scale_bilinear_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height);
""")

ffibuilder.set_source("_scaler_cffi",
"""
    #include "src/scaler.h"
""",
    libraries=['build/scaler'])

def build():
    ffibuilder.compile(verbose=True, target="scaler/_scaler_cffi.*")

if __name__ == "__main__":
    build()
