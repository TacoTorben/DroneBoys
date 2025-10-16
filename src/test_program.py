import cv2
import numpy as np
import og_pic 
from pathlib import Path
import time
import pandas as pd
import os

cmax = 270 #Canny max value, old value was 320
blurvalue = 11 #Median blur value, old value was 11
denoisevalue = 8 #Denoise value, old value was 18

base_dir = Path(__file__).resolve().parent
image_dir = base_dir.parent / "Images"
full_object_detect = 0
#objects in all 85
start_time = time.time()
size = (815,815) #Size to resize the image to
num_images = 30
img_arr = np.empty((8, 1), dtype=object) #Array to hold the images
results = [] 
for i in range(1, num_images + 1):
    img_path = image_dir / f"{i}.png"
    
    img = cv2.imread(str(img_path))
    rezised, edges, num_labels, closed, elapsed = og_pic.edgedetection.edgedetection(img, cmax, blurvalue, denoisevalue, start_time, size)
   
    cv2.imshow(f"PLS{i}", closed)
    print(f"detected {num_labels-1} objects")   
    print(f" At image {i}")
    results.append([round(elapsed,2), num_labels-1, i, size[0]])
    start_time = time.time()
    


df = pd.DataFrame(results, columns=["time", "objects", "image_number", "Changed Parameters"])

csv_path = "/home/dksoren/DroneBoys/src/object_results.csv"
df.to_csv(csv_path, index=False, sep=";")

print(f"✅ Saved results to {csv_path}")
cv2.waitKey(0)

#img_arr[0] = cv2.imread("/home/dksoren/DroneBoys/Images/11.png") #Path to the image
#img_arr[1] = cv2.imread("/home/snekkie/DroneBoys/Images/rb.jpg") #Path to the image
#img_arr[2] = cv2.imread("/home/snekkie/DroneBoys/Images/rbg.jpg") #Path to the image
#img_arr[3] = cv2.imread("/home/snekkie/DroneBoys/Images/r2bg.jpg") #Path to the image
#img_arr[4] = cv2.imread("/home/snekkie/DroneBoys/Images/3r2bg.jpg") #Path to the image
#img_arr[5] = cv2.imread("/home/snekkie/DroneBoys/Images/lg.jpg") #Path to the image
#img_arr[6] = cv2.imread("/home/snekkie/DroneBoys/Images/3lgrg.jpg") #Path to the image
#img_arr[7] = cv2.imread("/home/snekkie/DroneBoys/Images/flower.jpeg") #Path to the image
#

#rezised, edges, num_labels, closed = og_pic.edgedetection.edgedetection(img_arr[0], cmax, blurvalue, denoisevalue)
#cv2.imshow("PLS", closed)
#cv2.imshow("edges", edges)
#print(f"detected {num_labels-1} objects")

cv2.waitKey(0)

