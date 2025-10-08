import cv2  # Not actually necessary if you just want to create an image.
import numpy as np
import math
import colorsys
import Hist_of_H_value_def
witdh, height = 500, 500
blank_image = np.zeros((witdh,height,3), np.uint8)
blank_image[:,0:witdh] = (255, 255, 255)   
rgb_norm =  np.array([255, 255, 255])
img = cv2.imread("/home/dksoren/DroneBoys/Images/flower.jpeg")
resized_img = cv2.resize(img, (600,600))
 # (B, G, R)
H = 0.0
S = 0.5 
V = 1 


for j in range(0,720):
    new_H = np.round((j/2)/360,2)
    #print(colorsys.hsv_to_rgb(new_H,S,V))
    
    #print(rgb_255)
    #print(f"j = {j} cos = {math.cos(math.radians(j))}")
 
    for h in range(0, 200):
        r = h
        new_S = (r/100)/2
        #angle = math.cos(x_val)
        #print(angle)
        rgb = colorsys.hsv_to_rgb(new_H, new_S, V)  # returns tuple (r, g, b) in [0,1]

        # Convert to NumPy array
        rgb_array = np.array(rgb, dtype=np.float32)
        rgb_255 = (rgb_array * 255).astype(np.uint8)
        x_val_j = int(r * math.cos(math.radians((-j/2)+180))+250)
        y_val_j = int(r * math.sin(math.radians((-j/2)+180))+250)
        
        blank_image[x_val_j,y_val_j] = [rgb_255[2],rgb_255[1],rgb_255[0]]
        
blurred = cv2.medianBlur(blank_image, 15)

R_col = 39
G_col = 130
B_col = 29

upper_h = 0
lower_h = 0
test_color = colorsys.rgb_to_hsv(R_col, G_col, B_col) 

h_value = (Hist_of_H_value_def.oliver_biatch.get_dat(img)[0] *2)
print(h_value)
range = 80
if h_value > 360:
    upper_h = 360
else:
    upper_h = h_value + range

if h_value < 0:
    lower_h = 0
else:
   lower_h = h_value - range


#rbg_lower = np.array(colorsys.hsv_to_rgb(lower[0], lower[1], lower[2])) 
#rgb_upper = np.array(colorsys.hsv_to_rgb(upper[0], upper[1], upper[2])) 


hsv_lower = np.array([lower_h,40,40])
hsv_upper = np.array([upper_h,255,255])
hvs_img = cv2.cvtColor(resized_img, cv2.COLOR_BGR2HSV)
mask = cv2.inRange(hvs_img, hsv_lower, hsv_upper)
revers_mask = cv2.bitwise_not(mask)
blu = cv2.medianBlur(revers_mask, 15)
cv2.imshow("masked",  blu)
cv2.imshow("wtf", resized_img)
cv2.imshow("dick", blurred)
cv2.waitKey(0)