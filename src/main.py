
import cv2 
import numpy as np
import matplotlib.pyplot as plt

grid = np.zeros((10, 10))



img = cv2.imread('/home/droneboys/DroneBoys/Images/IMG_1361.jpg')   # you can read in images with opencv
img_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

print(img_hsv.shape)

hsv_color1 = np.asarray([150, 50, 70])   # white!
hsv_color2 = np.asarray([180, 255, 255])   # yellow! note the order

mask = cv2.inRange(img_hsv, hsv_color1, hsv_color2)
   # this colormap will display in black / white
#plt.imshow(mask, cmap='gray')

#plt.show()

params = cv2.SimpleBlobDetector_Params()
params.filterByColor = True
params.blobColor = 255
params.minArea = 10
params.maxArea = 100
params.filterByArea = True
params.filterByCircularity = True
params.minCircularity = 0.785


detector = cv2.SimpleBlobDetector_create(params)
keypoints = detector.detect(mask)
output = cv2.drawKeypoints(img, keypoints, np.array([]), (255,0,0),cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)

cv2.imshow("Dildo", output)
cv2.waitKey(0)
