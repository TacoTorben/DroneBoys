
import cv2 
import numpy as np
import matplotlib.pyplot as plt




img = cv2.imread("/home/dksoren/DroneBoys/Images/IMG_1361.jpg")   # you can read in images with opencv
img_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

print(img_hsv.shape)

hsv_color1 = np.asarray([150, 50, 70])   # white!
hsv_color2 = np.asarray([180, 255, 255])   # yellow! note the order

mask = cv2.inRange(img_hsv, hsv_color1, hsv_color2)
mask = cv2.inRange(img_hsv, hsv_color1, hsv_color2)
   # this colormap will display in black / white
#plt.imshow(mask, cmap='gray')
#
#plt.show()

median = cv2.medianBlur(mask, 11)  
#median_rgb = cv2.cvtColor(median, cv2.COLOR_BGR2RGB)  

plt.imshow(median)
plt.title('Median Blurred Image')
plt.axis('off')
plt.show()
