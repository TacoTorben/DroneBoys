import cv2
import numpy as np

class edgedetection:
    def edgedetection(img,cmax, blurvalue, denoisevalue):
        
        width, height = 500, 500 #Define the size of the IMAGE
        #img = cv2.imread("/home/snekkie/DroneBoys/Images/rbg.jpg") #Path to the image
        rezised = cv2.resize(img, (width, height)) #Resize the image to the defined size

        blur = cv2.medianBlur(rezised,blurvalue) #Apply median blur to the image
        denoised = cv2.fastNlMeansDenoisingColored(blur,None,denoisevalue,10,7,21) #Apply denoising to the image
        edges = cv2.Canny(denoised, 100, cmax) #Apply Canny edge detection to the image

        ys, xs = np.where(edges ==255) #Get the coordinates of the edges
        coords = np.column_stack((xs, ys)).astype(np.int32) #Stack the coordinates into a single array

        for i in range(int(len(coords))): #Loop through the coordinates
            for j in range(-1,2):
                for k in range(-1,2):
                    if coords[i][0]+j < width and coords[i][0]+j >= 0 and coords[i][1]+k < height and coords[i][1]+k >= 0:
                        rezised[coords[i][1]+k, coords[i][0]+j] = [255, 255, 255]
        return rezised

#cv2.imshow("PRESS Q",rezised) #Show the final image
#cv2.imshow("PLS", edges)
#cv2.imshow("denoised", denoised)
#cv2.imshow("blur", blur)
##cv2.imshow("img", img)
#cv2.waitKey(0)