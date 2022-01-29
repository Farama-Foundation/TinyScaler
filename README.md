# TinyScaler

A small CPU image scaling library with SIMD support on x86_64 and Arm (Neon).

## Requirements

- cmake >= 3.1

## Install

> pip install .

## Usage

```python
import scaler

# Load image somehow. Must be float32 with 4 channels to make full use of SIMD

# Scale image to 64x64 pixels
scaler.scaler_nearest(np_array_img.astype(np.float32), (64, 64)) # With nearest filtering
scaler.scaler_bilinear(np_array_img.astype(np.float32), (64, 64)) # With bilinear filtering

# Save image somehow
```

## License

MIT License, see [LICENSE.md](./LICENSE.md)



