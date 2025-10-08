
import cv2 
import numpy as np
import matplotlib.pyplot as plt




img = cv2.imread("/home/dksoren/DroneBoys/Images/IMG_1361.jpg")   # you can read in images with opencv
img_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

print(img_hsv.shape)

hsv_color1 = np.asarray([150, 50, 70])   # white!
hsv_color2 = np.asarray([180, 255, 255])   # yellow! note the order

#(101.2368, 125.0176, 117.74080000000001, 0.0)

lower = np.array([108, 115, 91], dtype=np.uint8)
upper = np.array([128, 135, 111], dtype=np.uint8)

mask = cv2.inRange(img_hsv, lower, upper)

   # this colormap will display in black / white
#plt.imshow(mask, cmap='gray')
#
#plt.show()

median = cv2.medianBlur(mask, 11)  
#median_rgb = cv2.cvtColor(median, cv2.COLOR_BGR2RGB)  

plt.imshow(mask)
plt.title('Median Blurred Image')
plt.axis('off')
plt.show()
rgb = np.array([101.2368, 125.0176, 117.7408], dtype=np.uint8)
onecolorimg = np.uint8([[rgb]]) 
def convert_to_HSI(image):
    # Convert BGR to float32 for precision
    bgr = image.astype(np.float32) / 255.0
    B, G, R = cv2.split(bgr)

    # Calculate Intensity
    I = (R + G + B) / 3.0

    # Calculate Saturation
    min_rgb = np.minimum(np.minimum(R, G), B)
    S = 1 - (3 / (R + G + B + 1e-6)) * min_rgb
    S[I == 0] = 0  # If intensity is zero, saturation is zero

    # Calculate Hue
    num = 0.5 * ((R - G) + (R - B))
    den = np.sqrt((R - G)**2 + (R - B) * (G - B)) + 1e-6 # Avoid division by zero by adding 0.000001
    theta = np.arccos(num / den)
    H = np.zeros_like(I)

    H[B <= G] = theta[B <= G]
    H[B > G] = (2 * np.pi) - theta[B > G]
    H = H / (2 * np.pi)  # Normalize to [0, 1]

    HSI = cv2.merge((H, S, I))
    return HSI
print(convert_to_HSI(onecolorimg))