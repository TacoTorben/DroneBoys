import cv2  # Not actually necessary if you just want to create an image.
import numpy as np
import math
import colorsys
witdh, height = 500, 500
blank_image = np.zeros((witdh,height,3), np.uint8)
blank_image[:,0:witdh] = (255, 255, 255)   
#rgb_norm =  np.array([255, 255, 255])
  # (B, G, R)
H = 0.0
S = 0.5 
V = 1 
print(f"S = {S} V = {V}")

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
        
blurred = cv2.medianBlur(blank_image, 11)
cv2.imshow("wtf", blurred)
cv2.waitKey(0)