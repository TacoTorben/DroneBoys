import cv2
import numpy as np
import time

class edgedetection:
    def edgedetection(img,cmax, blurvalue, denoisevalue,start_time, size):
        '''
        This function takes an image as input and resize, blur, denoise, edge detection, and morp to be able to give number of obejcts.
        '''
        #print(f"start time = {time.time() - start_time}")
         #Define the size of the IMAGE
        #img = cv2.imread("/home/snekkie/DroneBoys/Images/rbg.jpg") #Path to the image
        rezised = cv2.resize(img, (size)) #Resize the image to the defined size
        #print(f"resized time = {time.time() - start_time}")
        blur = cv2.medianBlur(rezised,blurvalue) #Apply median blur to the image
        #print(f"blur time = {time.time() - start_time}")
        denoised = cv2.fastNlMeansDenoisingColored(blur,None,denoisevalue,10,7,21) #Apply denoising to the image
        #print(f"denoised time = {time.time() - start_time}")
        edges = cv2.Canny(denoised, 100, cmax) #Apply Canny edge detection to the image
        #print(f"edges time = {time.time() - start_time}")
        ys, xs = np.where(edges ==255) #Get the coordinates of the edges
        coords = np.column_stack((xs, ys)).astype(np.int32) #Stack the coordinates into a single array
        #print(f"coords time = {time.time() - start_time}")
        for i in range(int(len(coords))): #Loop through the coordinates
            for j in range(-1,2):
                for k in range(-1,2):
                    if coords[i][0]+j < size[0] and coords[i][0]+j >= 0 and coords[i][1]+k < size[1] and coords[i][1]+k >= 0:
                        rezised[coords[i][1]+k, coords[i][0]+j] = [255, 255, 255]
        
        kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5))
        closed = cv2.morphologyEx(edges, cv2.MORPH_CLOSE, kernel, iterations=1)
        #print(f"morph time = {time.time() - start_time}")
        num_labels, labels, stats, centroids = cv2.connectedComponentsWithStats(closed)
        print(f"total time = {time.time() - start_time}")
        elapsed = time.time() - start_time
        return rezised, edges, num_labels, closed, elapsed

#rezised, edges = edgedetection.edgedetection(cv2.imread("/home/dksoren/DroneBoys/Images/rbg.jpg"), 270, 11, 18)







#cv2.imshow("PRESS Q",rezised) #Show the final image
#cv2.imshow("PLS", rezised)
#cv2.imshow("edges", closed) #Show the edges
#cv2.imshow("denoised", denoised)
#cv2.imshow("blur", blur)

cv2.waitKey(0)