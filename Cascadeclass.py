import cv2 
import numpy as np

img = cv2.imread("/home/snekkie/DroneBoys/Images/deadjesper2.JPG")
rezised = cv2.resize(img, (800, 600))

human_cascade = cv2.CascadeClassifier('/home/snekkie/DroneBoys/src/haarcascade_upperbody.xml')
gray = cv2.cvtColor(rezised, cv2.COLOR_BGR2GRAY)
humans = human_cascade.detectMultiScale(
    gray,
    scaleFactor=1.05,
    minNeighbors=3,
    minSize=(60, 120)
)

for (x, y, w, h) in humans:
    cv2.rectangle(rezised, (x, y), (x + w, y + h), (255, 0, 0), 2)

if human_cascade.empty():
    print("Cascade load FAILED")
    exit()


#cv2.imshow("original", img)
cv2.imshow("rezised", rezised)
cv2.waitKey(0)
