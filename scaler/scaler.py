import numpy as np
from _scaler_cffi import ffi, lib

def scaler_nearest(src : np.ndarray, size : tuple):
    assert(len(src.shape) == 3 and src.shape[2] == 4) # Must be 4 channel

    length = size[0] * size[1] * 4

    dst = np.ascontiguousarray(np.empty(length, dtype=np.float32))

    src_cptr = ffi.cast("f32*", ffi.from_buffer(np.ascontiguousarray(src)))
    dst_cptr = ffi.cast("f32*", ffi.from_buffer(dst))

    lib.scaler_nearest_4f32(src_cptr, dst_cptr, src.shape[0], src.shape[1], size[0], size[1])

    return dst.reshape((size[0], size[1], 4))

def scaler_bilinear(src : np.ndarray, size : tuple):
    assert(len(src.shape) == 3 and src.shape[2] == 4) # Must be 4 channel

    length = size[0] * size[1] * 4

    dst = np.ascontiguousarray(np.empty(length, dtype=np.float32))

    src_cptr = ffi.cast("f32*", ffi.from_buffer(np.ascontiguousarray(src)))
    dst_cptr = ffi.cast("f32*", ffi.from_buffer(dst))

    lib.scaler_bilinear_4f32(src_cptr, dst_cptr, src.shape[0], src.shape[1], size[0], size[1])

    return dst.reshape((size[0], size[1], 4))
