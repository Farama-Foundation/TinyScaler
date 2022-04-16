import numpy as np
from ._scaler_cffi import ffi, lib

auto_convert = True # Global controlling whether automatic channel/type conversions take place

def _scale_4f32(src : np.ndarray, size : tuple, mode='bilinear', dst : np.ndarray = None):
    assert(len(src.shape) == 3 and src.shape[2] == 4) # Must be 4 channel

    if dst is None:
        length = size[0] * size[1] * 4

        dst = np.empty(length, dtype=np.float32)
    else:
        if len(dst.shape) != 3 or dst.shape[0] != size[0] or dst.shape[1] != size[1] or dst.shape[2] != 4:
            raise Exception('Incorrect dst size!')
        elif dst.dtype != np.float32:
            raise Exception('Incorrect dst type (must be float32)!')

    src_cptr = ffi.cast('f32*', ffi.from_buffer(np.ascontiguousarray(src)))
    dst_cptr = ffi.cast('f32*', ffi.from_buffer(np.ascontiguousarray(dst)))

    if mode == 'bilinear':
        lib.scale_bilinear_4f32(src_cptr, dst_cptr, src.shape[0], src.shape[1], size[0], size[1])
    else:
        lib.scale_nearest_4f32(src_cptr, dst_cptr, src.shape[0], src.shape[1], size[0], size[1])

    return dst.reshape((size[0], size[1], 4))

def scale(src : np.ndarray, size : tuple, mode='bilinear', dst : np.ndarray = None):
    '''
    scale (resize) a source image to a specified size

    Parameters
    ----------
    src : numpy.ndarray
        the source array, must have len(src.shape) == 2 or len(src.shape) == 3. Must be contiguous (using numpy.ascontiguousarray if not already).
        Ideally (most efficient) the shape is (width, height, 4) with dtype=numpy.float32 (others will cause a conversion to occur)
    size :
        target size, a tuple of two positive integers (width, height)
    mode : {'bilinear', 'nearest'}, optional
        interpolation method to use. Defaults to bilinar, can also be nearest
    dst : {None}, optional
        destination buffer to put resized image in. Leaving this = None will result in an allocation.
        For efficient code, set dst to a buffer of shape (size[0], size[1], 4) with dtype=numpy.float32

    Returns
    -------
    numpy.ndarray
        the scaled image

    Raises
    ------
    Exception
        when src is not contiguous or the wrong shape
    '''

    if not src.data.contiguous:
        raise Exception('Input image must be contiguous!')

    src_channels = 4
    src_type = src.dtype

    # Automatic conversion
    if auto_convert:
        if len(src.shape) != 3:
            raise Exception('Incorrect number of dimensions - need 3, received ' + str(len(src.shape)))

        if src.dtype != np.float32:
            if src.dtype == np.uint8:
                src = src.astype(np.float32)

                src *= float(1.0 / 255.0)
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
        result = _scale_4f32(src, size, mode, dst)
    except Exception as e:
        raise e

    # Covert back
    if auto_convert:
        if src_type == np.uint8:
            result = (result * float(255.0)).astype(np.uint8)[:, :, :src_channels]
        else:
            result = result[:, :, :src_channels].astype(src_type)

    return result
