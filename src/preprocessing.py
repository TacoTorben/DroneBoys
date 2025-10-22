import cv2
import numpy as np
import time
from pathlib import Path
import matplotlib.pyplot as plt
import math

base_dir = Path(__file__).resolve().parent
image_dir = base_dir.parent / "Images"
img_path_one = image_dir / f"4.1.06.tiff"
img_path_two = image_dir / f"4.1.02.tiff"
img_one = cv2.imread(str(img_path_one))
img_two = cv2.imread(str(img_path_two))


target_intensity = 100
resized = cv2.resize(img_one, (600,600))


def determine_intensity(image):
    gray_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    average_intensity = np.mean(gray_image)
    return average_intensity

def find_gamma_value(image, target_intensity):
    gray_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    current_intensity = np.mean(gray_image)
    if current_intensity == 0:
        return None
    gamma_value = math.log(target_intensity / 255) / math.log(current_intensity / 255)
    return gamma_value


class visual:
    def plot_histogram(image, title):
        gray_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        hist = cv2.calcHist([gray_image], [0], None, [256], [0, 256])
        plt.plot(hist)
        plt.title(title)
        plt.xlabel('Pixel Intensity')
        plt.ylabel('Frequency')
        plt.show()

gamma_corrected = np.array(255*(resized / 255) ** find_gamma_value(resized, target_intensity), dtype = 'uint8')
print(determine_intensity(gamma_corrected))
cv2.imshow('gamma_transformed'+str(find_gamma_value(resized, target_intensity))+'.jpg', gamma_corrected)


# Save the output.

cv2.imshow("Original Image", resized)
#cv2.imshow("Other original", cv2.resize(img_two, (600,600)))
gray_original = cv2.cvtColor(resized, cv2.COLOR_BGR2GRAY)
gray_gamma = cv2.cvtColor(gamma_corrected, cv2.COLOR_BGR2GRAY)

# Compute histograms
hist_original = cv2.calcHist([gray_original], [0], None, [256], [0, 256])
hist_gamma = cv2.calcHist([gray_gamma], [0], None, [256], [0, 256])
print(determine_intensity(resized))

# Plot


cv2.waitKey(0)


