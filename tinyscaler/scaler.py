import numpy as np
from ._scaler_cffi import ffi, lib

auto_convert = True # Global controlling whether automatic channel/type conversions take place

def _scale_nearest_4f32(src : np.ndarray, size : tuple, dst : np.ndarray = None):
    assert(len(src.shape) == 3 and src.shape[2] == 4) # Must be 4 channel

    if dst is None:
        length = size[0] * size[1] * 4

        dst = np.ascontiguousarray(np.empty(length, dtype=np.float32))
    elif len(dst.shape) != 3 or dst.shape[0] != size[0] or dst.shape[1] != size[1] or dst.shape[2] != 4:
        raise Exception('Incorrect dst size!')

    src_cptr = ffi.cast('f32*', ffi.from_buffer(np.ascontiguousarray(src)))
    dst_cptr = ffi.cast('f32*', ffi.from_buffer(dst))

    lib.scale_nearest_4f32(src_cptr, dst_cptr, src.shape[0], src.shape[1], size[0], size[1])

    return dst.reshape((size[0], size[1], 4))

def _scale_bilinear_4f32(src : np.ndarray, size : tuple, dst : np.ndarray = None):
    assert(len(src.shape) == 3 and src.shape[2] == 4) # Must be 4 channel

    if dst is None:
        length = size[0] * size[1] * 4

        dst = np.ascontiguousarray(np.empty(length, dtype=np.float32))
    elif len(dst.shape) != 3 or dst.shape[0] != size[0] or dst.shape[1] != size[1] or dst.shape[2] != 4:
        raise Exception('Incorrect dst size!')

    src_cptr = ffi.cast('f32*', ffi.from_buffer(np.ascontiguousarray(src)))
    dst_cptr = ffi.cast('f32*', ffi.from_buffer(dst))

    lib.scale_bilinear_4f32(src_cptr, dst_cptr, src.shape[0], src.shape[1], size[0], size[1])

    return dst.reshape((size[0], size[1], 4))

def scale(src : np.ndarray, size : tuple, mode='bilinear', dst : np.ndarray = None):
    src_channels = 4
    src_type = src.dtype

    # Automatic conversion
    if dst is None and auto_convert:
        if len(src.shape) != 3:
            raise Exception('Incorrect number of dimensions - need 3, received ' + str(len(src.shape)))

        if src.dtype != np.float32:
            if src.dtype == np.uint8:
                src = src / 255.0
            else:
                src = src.astype(np.float32)

        src_channels = src.shape[2]

        if src.shape[2] == 1: # Gray
            src_new = np.empty((src.shape[0], src.shape[1], 4), dtype=np.float32)
            src_new[:, :, 0] = src
            src_new[:, :, 1] = src
            src_new[:, :, 2] = src
            src_new[:, :, 3] = src

            src = src_new
        elif src.shape[2] == 3: # RGB
            src_new = np.empty((src.shape[0], src.shape[1], 4), dtype=np.float32)
            src_new[:, :, :3] = src
            src_new[:, :, 3] = np.ones((src.shape[0], src.shape[1]))

            src = src_new
        elif src.shape[2] != 4:
            raise Exception('Passed an invalid number of channels, must be 1, 3, or 4 (received ' + str(src.shape[2]) + ')!')
        
    result = None

    try:
        if mode == 'bilinear':
            result = _scale_bilinear_4f32(src, size, dst)
        else:
            result = _scale_nearest_4f32(src, size, dst)
    except Exception as e:
        raise e

    # Covert back
    if auto_convert:
        if src_type == np.uint8:
            result = (result * 255.0).astype(np.uint8)[:, :, :src_channels]
        else:
            result = result[:, :, :src_channels].astype(src_type)

    return result
