import numpy as np
import cv2

#picture mist be binary to work with blob detection
# Read and binarize
img = cv2.imread("shapes.png", cv2.IMREAD_GRAYSCALE)
# Prettyfy picture
_, img = cv2.threshold(img, 0, 255, cv2.THRESH_BINARY)

cv2.imshow("Original", img)

#specify expected max amount of blobs in picture to make array big enough to hold each blob
MAX_BLOBS = 10
blobs = [None] * (MAX_BLOBS + 1)

# function executing the grassfire algorithm while saving each blobs coordinates
def grassfire(image, start_coordinate, id, blobs_store):
    #enter first coordinate into burn queue
    y, x = start_coordinate

    #if coordinate is not 255 exit function and return current id
    if image[y, x] != 255:
        return id

    #define resolution of image, define burn queue with current coords, define coords array to save all coords for blob
    h, w = image.shape
    burn_queue = [(y,x)]
    coords = []

    #start grassfire method
    while burn_queue:
        #delete last coordinate in burn queue and jump to them
        y, x = burn_queue.pop()
        if image[y, x] != 255:
            continue

        #burn current pixel, and save in coords
        image[y, x] = id
        coords.append((y,x))

        #check neighbors for hit
        if x+1 < w and image[y, x+1] == 255:
            burn_queue.append((y,x+1))
        if y+1 < h and image[y+1, x] == 255:
            burn_queue.append((y+1,x))
        if x-1 >= 0 and image[y, x-1] == 255:
            burn_queue.append((y,x-1))
        if y-1 >= 0 and image[y-1, x] == 255:
            burn_queue.append((y-1,x))

    #Save coords of blob in blobs_store at current id index
    blobs_store[id] = np.array(coords)
    return id+1

#specify start id (must be minimum 1 and max 255)(keep at the current to not break later ranges)
next_id = 1
h, w = img.shape

#loop through each pixel and try grassfire algorithm
for y in range(h):
    for x in range(w):
        if next_id > MAX_BLOBS:
            print("Max blobs reached")
            break

        if img[y, x] == 255:  # start of a new blob
            next_id = grassfire(img, (y, x), next_id, blobs)

        if next_id > MAX_BLOBS:
            print("Max blobs reached")
            break

#prettyfy the visualisation of the grassfire algorithm
vis = (img.astype(np.float32) * (255.0 / max(1, next_id - 1))).astype(np.uint8)
cv2.imshow("Processed", vis)

#define max and min of each coordinate pr. blob to store the max and min values of each set of coordinates
maxx, maxy, minx, miny = [None]*(MAX_BLOBS+1),[None]*(MAX_BLOBS+1),[None]*(MAX_BLOBS+1),[None]*(MAX_BLOBS+1)

#loop through each blob and calculate and save each max and min value pr. coordinate
for i in range(next_id + 1):
    #create "coords" array with each coordinate of each blob
    coords = blobs[i]
    #ignore empty arrays
    if coords is None or len(coords) == 0:
        continue

    #save each y and x coord in seperate arrays
    ys = coords[:,0]
    xs = coords[:,1]

    #find max and min values of y and x coords in each blob
    maxy[i] = ys.max()
    miny[i] = ys.min()
    maxx[i] = xs.max()
    minx[i] = xs.min()

#specify each picture to visualize process
label = cv2.cvtColor(vis, cv2.COLOR_GRAY2BGR)
rec = cv2.cvtColor(vis, cv2.COLOR_GRAY2BGR)

#loop through each blob to add number and rectangle to blob
for i in range(next_id + 1):
    #ignore empty arrays
    if minx[i] is None:
        continue

    #find center of each blob
    cy = int((miny[i] + maxy[i])/2)
    cx = int((minx[i] + maxx[i])/2)

    #place number and rectangle on each blob
    #the +-10 os offsets because of the cv2.putText places the bottom right of the text at the coordinates.
    #workarounds recuire cv2.getTextSize but is not required if you can guess correctly.
    cv2.putText(label,f"{i}",(cx- 10,cy + 10),cv2.FONT_HERSHEY_SIMPLEX,1,(0,0,255),2)
    cv2.putText(rec, f"{i}", (cx - 10, cy + 10), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
    cv2.rectangle(rec, (minx[i], miny[i]), (maxx[i], maxy[i]), (0, 255, 0), 2)

cv2.imshow("Labeled", label)
cv2.imshow("Bounding boxes", rec)
cv2.waitKey(0)
cv2.destroyAllWindows()