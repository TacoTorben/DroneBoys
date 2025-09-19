
import cv2 
import numpy as np
import matplotlib.pyplot as plt

grid = np.zeros((10, 10))



img = cv2.imread("/home/dksoren/DroneBoys/Images/dorte.jpg")   # you can read in images with opencv
img = cv2.resize(img, (640, 480))
img_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

print(img_hsv.shape)

hsv_color1 = np.asarray([150, 50, 70])   # white!
hsv_color2 = np.asarray([180, 255, 255])   # yellow! note the order

mask = cv2.inRange(img_hsv, hsv_color1, hsv_color2)
   # this colormap will display in black / white
#plt.imshow(mask, cmap='gray')




median = cv2.medianBlur(mask, 11)  
#median_rgb = cv2.cvtColor(median, cv2.COLOR_BGR2RGB)  


#plt.title('Median Blurred Image')
#plt.axis('off')
#plt.show()

x_spot_lowest = 0
y_spot_lowest = 0
x_spot_highest = 0
y_spot_highest = 0

for y, row in enumerate(median):
    for x, pixel in enumerate(row):
         if pixel == 255:
               # check surrounding pixels
               white_pixel_count = 0
               for j in range(-5, 6):
                  for i in range(-5, 6):
                     if y+j >= 0 and y+j < median.shape[0] and x+i >= 0 and x+i < median.shape[1]:
                           if median[y+j, x+i] > 200:
                              white_pixel_count += 1
                              if x < x_spot_lowest or x_spot_lowest == 0:
                                 x_spot_lowest = x
                              if x > x_spot_highest:
                                 x_spot_highest = x
                              if y < y_spot_lowest or y_spot_lowest == 0:
                                 y_spot_lowest = y
                              if y > y_spot_highest:
                                 y_spot_highest = y
    

                  #   for i in range(1, 10):
                  #      img[int(average_y)+i, int(average_x)+j] = [255, 0, 0]
                           
            ##print(f"value at {x} and {y} = {pixel}")
            #for j in range(1, 10):
            #    for i in range(1, 10):
            #        img[30+i, 100+j] = [255, 0, 0]
square_x = x_spot_highest - x_spot_lowest
square_y = y_spot_highest - y_spot_lowest
for j in range(square_y):
      for i in range(1,3):
         img[y_spot_lowest + j, x_spot_lowest+i] = [255, 0, 0]
         img[y_spot_lowest + j, x_spot_highest+i] = [255, 0, 0]
for i in range(square_x):
    for j in range(1,3):
      img[y_spot_lowest+j, x_spot_lowest + i] = [255, 0, 0]
      img[y_spot_highest+j, x_spot_lowest + i] = [255, 0, 0]

print(f"Lowest X: {x_spot_lowest}, Highest X: {x_spot_highest},s Lowest Y: {y_spot_lowest}, Highest Y: {y_spot_highest}")             
#plt.imshow(median)
#plt.show()



#params = cv2.SimpleBlobDetector_Params()
#params.filterByColor = True
#params.blobColor = 255
#params.minArea = 10
#params.maxArea = 100
#params.filterByArea = True
#params.filterByCircularity = True
#params.minCircularity = 0.785
#
#
#detector = cv2.SimpleBlobDetector_create(params)
#keypoints = detector.detect(mask)
#output = cv2.drawKeypoints(img, keypoints, np.array([]), (255,0,0),cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)
#
cv2.imshow("Dildo", img)
cv2.waitKey(0)
