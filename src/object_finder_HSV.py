import cv2  # Not actually necessary if you just want to create an image.
import numpy as np
import math
import colorsys
import Hist_of_H_value_def
import os
from pathlib import Path
import time
class ObjectFinderHSV:
    def objecfinder(img, start_time, range):
        #base_dir = Path(__file__).resolve().parent
        #image_dir = base_dir.parent / "Images"
        #img_path = image_dir / f"30.png"
#
        #img = cv2.imread(str(img_path))
        resized_img = cv2.resize(img, (600,600))
        V = 1

        upper_h = 0
        lower_h = 0
        h_value = (Hist_of_H_value_def.average_hue.calculate_average_hue(img)[0] *2)
    
        hue_range = range            #The range for hue adjustment
        if h_value > 360:           #To make sure we wrap around the hue wheel
            upper_h = 360
        else:
            upper_h = h_value + hue_range
        if h_value < 0:
            lower_h = 0
        else:
           lower_h = h_value - hue_range
        

        #To get a range of saturation and value
        hsv_lower = np.array([lower_h,40,40])
        hsv_upper = np.array([upper_h,255,255])



        hvs_img = cv2.cvtColor(resized_img, cv2.COLOR_BGR2HSV) # To get the color as an hsv image

        mask = cv2.inRange(hvs_img, hsv_lower, hsv_upper) #Mask it with our range, thereby masking it with the "average" color
        
        revers_mask = cv2.bitwise_not(mask)
        blu = cv2.medianBlur(revers_mask, 15)
        num_labels, labels_im, stats, centroids = cv2.connectedComponentsWithStats(blu)
        stats = stats[1:]
        centroids = centroids[1:]
        num_labels = num_labels - 1
        for (cx, cy) in centroids:
                    cv2.circle(resized_img, (int(cx), int(cy)), 40, (0, 0, 255), 3)
        print(f"total time = {time.time() - start_time}")
        elapsed = time.time() - start_time
        return num_labels, elapsed, mask , hvs_img, blu, revers_mask

#cv2.imshow("Blu", blu)
#cv2.waitKey(0)
    
