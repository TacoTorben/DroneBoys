import cv2 as cv

img = cv.imread("/home/dksoren/DroneBoys/Images/flower.jpeg")
og_img = img.copy()
if img is None:
	raise FileNotFoundError("Image not found at the specified path.")
b, g, r = cv.split(img)

img[:,:,2] = 0

rezised_img = cv.resize(img, (640, 480))

removed_image = cv.resize(cv.add(og_img,img), (640, 480))

cv.imshow("Dicks",rezised_img)
cv.waitKey(0)