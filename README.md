# TinyScaler

A small CPU image scaling library with SIMD support on x86_64 and Arm (Neon).

## Requirements

- cmake >= 3.1

## Install

> pip install .

## Usage

```python
import numpy as np
import tinyscaler

img = np.random.rand(8, 8, 4)

print(tinyscaler.scale_nearest(img, (64, 64)))
print(tinyscaler.scale_bilinear(img, (64, 64)))
```

## License

MIT License, see [LICENSE.md](./LICENSE.md)



