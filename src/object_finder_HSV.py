import cv2  # Not actually necessary if you just want to create an image.
import numpy as np
import math
import colorsys
import Hist_of_H_value_def



img = cv2.imread("/home/dksoren/DroneBoys/Images/flower.jpeg")
resized_img = cv2.resize(img, (600,600))
V = 1

upper_h = 0
lower_h = 0
h_value = (Hist_of_H_value_def.oliver_biatch.get_dat(img)[0] *2)
range = 80
if h_value > 360:
    upper_h = 360
else:
    upper_h = h_value + range 
if h_value < 0:
    lower_h = 0
else:
   lower_h = h_value - range

hsv_lower = np.array([lower_h,40,40])
hsv_upper = np.array([upper_h,255,255])
hvs_img = cv2.cvtColor(resized_img, cv2.COLOR_BGR2HSV)
mask = cv2.inRange(hvs_img, hsv_lower, hsv_upper)
revers_mask = cv2.bitwise_not(mask)
blu = cv2.medianBlur(revers_mask, 15)


cv2.imshow("Blu", blu)
cv2.waitKey(0)