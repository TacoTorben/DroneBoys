import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt

img = cv.imread("/home/jesper-kwame-jensen/DroneBoys/Images/flower.jpeg")   # you can read in images with opencv
if img is None:
    raise FileNotFoundError("Image not found at the specified path.")

img = cv.resize(img, (640, 480))

# Convert to grayscale
gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)

# Create a named window
cv.namedWindow('Canny Edge Detection')

# Create trackbars (sliders) for thresholds
cv.createTrackbar('Threshold1', 'Canny Edge Detection', 50, 1000, lambda x: None)
cv.createTrackbar('Threshold2', 'Canny Edge Detection', 150, 1000, lambda x: None)

while True:
    # Get the current positions of the trackbars
    thresh1 = cv.getTrackbarPos('Threshold1', 'Canny Edge Detection')
    thresh2 = cv.getTrackbarPos('Threshold2', 'Canny Edge Detection')

    # Apply Canny edge detection to the grayscale image
    edges = cv.Canny(gray, thresh1, thresh2)

    # Display the edge-detected image
    cv.imshow('Canny Edge Detection', edges)

    # Force GUI update and break on ESC
    if cv.waitKey(1) & 0xFF == 27:
        break
    if cv.getWindowProperty('Canny Edge Detection', 0) < 0: #Forces GUI update
        break


cv.destroyAllWindows()


#cv.imshow("Dicks",rezised_img)
cv.waitKey(0)