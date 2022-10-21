<p align="center">
    <img src="https://raw.githubusercontent.com/Farama-Foundation/TinyScaler/main/tinyscaler-text.png" width="500px"/>
</p>

A small CPU image scaling library with SIMD support on x86_64 and Arm (Neon). This project is aimed to replace OpenCV for image resizing, resolving installation inconveniences and compatibility issues. We developed this for future use in Gymnasium and PettingZoo wrappers.

## Usage
Tinyscaler contains a single external function, `scale` that using a numpy array input for the image and the new resized shape, returns the resized image. 

```python
import numpy as np
import tinyscaler

img = np.random.rand(64, 64, 4).astype(np.float32)

resize_img = tinyscaler.scale(img, (32, 32))
print(resize_img.shape, resize_img.dtype)  # (32, 32) np.float32
```

## Installation
You can install from PyPI using `pip install tinyscaler`. Linux and macOS with Python 3.7, 3.8, 3.9 and 3.10 are supported.

## Performance

In a [simple benchmark](./examples/benchmark.py), we resized the same image (4928x3279) down to (852x567) 100 times using bilinear filtering with several libraries. Here are the times (in seconds) spent (measured with Python's perf_counter) on a AMD 1950x:

```
Time elapsed for tinyscaler: 0.7968465110002398
Time elapsed for OpenCV: 0.48667862100001
Time elapsed for Pillow: 12.672875003999707
Time elapsed for skimage: 164.45401711399973
```

And with area filtering (just TinyScaler and OpenCV):

```
Time elapsed for tinyscaler: 4.34793155800071
Time elapsed for OpenCV: 8.118138265999733
```

All methods were forced to use a single thread. OpenCV is slightly faster than TinyScaler for bilinear filtering, but TinyScaler remains very fast regardless.

Interestingly, for area filtering, TinyScaler is faster (almost 2x).

