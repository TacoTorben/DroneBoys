import cv2
import numpy as np
import matplotlib.pyplot as plt


img = cv2.imread("/home/dksoren/DroneBoys/Images/100075_lowres.jpg", cv2.IMREAD_GRAYSCALE)

min_val = np.min(img)
max_val = np.max(img)

# Apply contrast stretching
stretched = ((img - min_val) / (max_val - min_val) * 255).astype(np.uint8)
min_val = np.min(stretched)
max_val = np.max(stretched)

stretched_twice = ((stretched - min_val) / (max_val - min_val) * 255).astype(np.uint8)

plt.figure(figsize=(12, 10))

plt.subplot(2, 2, 1)
plt.title("Stretched Image")
plt.imshow(stretched, cmap="gray")
plt.axis("off")

plt.subplot(2, 2, 3)
plt.title("Original Image")
plt.imshow(stretched_twice, cmap="gray")
plt.axis("off")

plt.subplot(2, 2, 4)
plt.title("Stretched Histogram")
plt.hist(stretched_twice.ravel(), bins=256, range=[0, 256])
plt.xlim([0, 255])

plt.subplot(2, 2, 2)
plt.title("Histogram")
plt.hist(stretched.ravel(), bins=256, range=[0, 256])
plt.xlim([0, 255])

plt.tight_layout()
plt.show()