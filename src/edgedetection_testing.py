import cv2
import numpy as np
import og_pic 


cmax = 270 #Canny max value, old value was 320
blurvalue = 11 #Median blur value, old value was 11
denoisevalue = 8 #Denoise value, old value was 18



img_arr = [0,0,0,0,0,0,0,0,0,0]
img_arr[0] = cv2.imread("/home/snekkie/DroneBoys/Images/r.jpg") #Path to the image
img_arr[1] = cv2.imread("/home/snekkie/DroneBoys/Images/rb.jpg") #Path to the image
img_arr[2] = cv2.imread("/home/snekkie/DroneBoys/Images/rbg.jpg") #Path to the image
img_arr[3] = cv2.imread("/home/snekkie/DroneBoys/Images/r2bg.jpg") #Path to the image
img_arr[4] = cv2.imread("/home/snekkie/DroneBoys/Images/3r2bg.jpg") #Path to the image
img_arr[5] = cv2.imread("/home/snekkie/DroneBoys/Images/lg.jpg") #Path to the image
img_arr[6] = cv2.imread("/home/snekkie/DroneBoys/Images/3lgrg.jpg") #Path to the image
img_arr[7] = cv2.imread("/home/snekkie/DroneBoys/Images/flower.jpeg") #Path to the image


cv2.imshow("Rd",og_pic.edgedetection.edgedetection(img_arr[0], cmax, blurvalue, denoisevalue))
cv2.imshow("Rd bl",og_pic.edgedetection.edgedetection(img_arr[1], cmax, blurvalue, denoisevalue))
cv2.imshow("Rd bl gul",og_pic.edgedetection.edgedetection(img_arr[2], cmax, blurvalue, denoisevalue))
cv2.imshow("Rd 2bl gul",og_pic.edgedetection.edgedetection(img_arr[3], cmax, blurvalue, denoisevalue))
cv2.imshow("3Rd 2bl gul",og_pic.edgedetection.edgedetection(img_arr[4], cmax, blurvalue, denoisevalue))
cv2.imshow("Lysgrn",og_pic.edgedetection.edgedetection(img_arr[5], cmax, blurvalue, denoisevalue))
cv2.imshow("3Lysgrn rd gul",og_pic.edgedetection.edgedetection(img_arr[6], cmax, blurvalue, denoisevalue))
cv2.imshow("flower",og_pic.edgedetection.edgedetection(img_arr[7],cmax, blurvalue, denoisevalue))

cv2.waitKey(0)