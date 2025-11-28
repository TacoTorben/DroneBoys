import cv2
import numpy as np

image = cv2.imread("/home/snekkie/DroneBoys/Images/deadjesper.JPG")
if image is None:
    raise SystemExit("Failed to load image.")
img = cv2.GaussianBlur(image, (5, 5), 0)

while True:
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    
    red_lower1 = np.array([0, 100, 100])
    red_upper1 = np.array([10, 255, 255])
    red_mask1 = cv2.inRange(hsv, red_lower1, red_upper1)

    green_lower = np.array([100, 100, 100])
    green_upper = np.array([255, 255, 255])
    green_mask = cv2.inRange(hsv, green_lower, green_upper)

    blue_lower = np.array([100, 100, 100])
    blue_upper = np.array([140, 255, 255])
    blue_mask = cv2.inRange(hsv, blue_lower, blue_upper)

    kernal = np.ones((5, 5), "uint8")

    #Detecting red color
    red_mask1 = cv2.dilate(red_mask1, kernal)
    res_red = cv2.bitwise_and(img, img, mask = red_mask1)

    #Detecting green color
    green_mask = cv2.dilate(green_mask, kernal)
    res_green = cv2.bitwise_and(img, img, mask = green_mask)

    #Detecting blue color
    blue_mask = cv2.dilate(blue_mask, kernal)
    res_blue = cv2.bitwise_and(img, img, mask = blue_mask)

    #Contours for red color
    contours, hierarchy = cv2.findContours(red_mask1, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    for pic, contour in enumerate(contours):
        area = cv2.contourArea(contour)
        if(area > 300):
            x, y, w, h = cv2.boundingRect(contour)
            img = cv2.rectangle(img, (x, y), (x + w, y + h), (0, 0, 255), 2)
            cv2.putText(img, "Red Colour", (x, y), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255))
    
    #Contours for green color
    contours, hierarchy = cv2.findContours(green_mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    for pic, contour in enumerate(contours):
        area = cv2.contourArea(contour)
        if(area > 300):
            x, y, w, h = cv2.boundingRect(contour)
            img = cv2.rectangle(img, (x, y), (x + w, y + h), (0, 255, 0), 2)
            cv2.putText(img, "Green Colour", (x, y), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0))

    #Contours for blue color
    contours, hierarchy = cv2.findContours(blue_mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    for pic, contour in enumerate(contours):
        area = cv2.contourArea(contour)
        if(area > 300):
            x, y, w, h = cv2.boundingRect(contour)
            img = cv2.rectangle(img, (x, y), (x + w, y + h), (255, 0, 0), 2)
            cv2.putText(img, "Blue Colour", (x, y), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 0, 0))
    #DISPLAY
    cv2.imshow("Color Detection", img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
    

