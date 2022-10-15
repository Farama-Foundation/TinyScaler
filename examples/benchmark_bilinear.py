# Benchmark between tinyscaler, OpenCV, Pillow, and skImage using bilinear filtering
import time

import cv2
import numpy as np
import tinyscaler
from PIL import Image
from skimage.transform import resize

# Disable multithreading and GPU support for OpenCV for a single-threaded CPU comparison
cv2.setNumThreads(1)
cv2.ocl.setUseOpenCL(False)

# Number of scales to perform
numScales = 100

# Loading this image: https://github.com/Cykooz/fast_image_resize/blob/main/data/nasa-4928x3279.png
img8 = cv2.cvtColor(cv2.imread("nasa-4928x3279.png"), cv2.COLOR_BGR2RGBA)
img = (img8 / 255.0).astype(np.float32)  # Preferred format

targetSize = (852, 567)

dst = np.empty((targetSize[1], targetSize[0], 4), dtype=np.float32)

start = time.perf_counter()

for t in range(numScales):
    tinyscaler.scale(img, targetSize, mode="bilinear", dst=dst)

end = time.perf_counter()

print("Time elapsed for tinyscaler: " + str(end - start))

# Save the result from tinyscaler for viewing
cv2.imwrite(
    "result.png", cv2.cvtColor((dst * 255.0).astype(np.uint8), cv2.COLOR_RGBA2BGR)
)

start = time.perf_counter()

for t in range(numScales):
    cv2.resize(img, targetSize, dst=dst, interpolation=cv2.INTER_LINEAR)

end = time.perf_counter()

cv2.imwrite(
    "result_cv.png", cv2.cvtColor((dst * 255.0).astype(np.uint8), cv2.COLOR_RGBA2BGR)
)

print("Time elapsed for OpenCV: " + str(end - start))

pimg = Image.fromarray(img8)

start = time.perf_counter()

for t in range(numScales):
    pimg.resize(targetSize, Image.Resampling.BILINEAR)

end = time.perf_counter()

print("Time elapsed for Pillow: " + str(end - start))

start = time.perf_counter()

for t in range(numScales):
    resize(img, targetSize)

end = time.perf_counter()

print("Time elapsed for skimage: " + str(end - start))
