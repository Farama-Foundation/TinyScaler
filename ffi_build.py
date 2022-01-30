from cffi import FFI
import os.path

def build(lib_path, ext_path):
    ffi = FFI()

    ffi.cdef('''
        typedef unsigned char u8;
        typedef int i32;
        typedef float f32;
        typedef double f64;

        void scale_nearest_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height);
        void scale_bilinear_4f32(f32 src[], f32 dst[], i32 src_width, i32 src_height, i32 dst_width, i32 dst_height);
    ''')

    ffi.set_source('_scaler_cffi',
        '''
            #include "src/scaler.h"
        ''',
        library_dirs=[lib_path],
        libraries=['TinyScaler'],
    )

    ffi.compile(target=os.path.join(ext_path, 'tinyscaler/_scaler_cffi.*'))
