
import cv2 
import numpy as np
import matplotlib.pyplot as plt

grid = np.zeros((10, 10))

rezised_height = 100
rezised_width = 100

img = cv2.imread("/home/dksoren/DroneBoys/Images/flower.jpeg")
og_image = img.copy()   # you can read in images with opencv
img = cv2.resize(img, (640, 480))
og_image = cv2.imread("/home/dksoren/DroneBoys/Images/flower.jpeg")
img_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
resized_img2 = cv2.resize(og_image, (25,25))
print(img_hsv.shape)

hsv_color1 = np.asarray([150, 50, 70])   # white!
hsv_color2 = np.asarray([180, 255, 255])   # yellow! note the order

mask = cv2.inRange(img_hsv, hsv_color1, hsv_color2)
   # this colormap will display in black / white
#plt.imshow(mask, cmap='gray')




median = cv2.medianBlur(mask, 11)  
#median_rgb = cv2.cvtColor(median, cv2.COLOR_BGR2RGB)  


#plt.title('Median Blurred Image')
#plt.axis('off')
#plt.show()

x_spot_lowest = 0
y_spot_lowest = 0
x_spot_highest = 0
y_spot_highest = 0
def convert_to_HSI(image):
    # Convert BGR to float32 for precision
    bgr = image.astype(np.float32) / 255.0
    B, G, R = cv2.split(bgr)

    # Calculate Intensity
    I = (R + G + B) / 3.0

    # Calculate Saturation
    min_rgb = np.minimum(np.minimum(R, G), B)
    S = 1 - (3 / (R + G + B + 1e-6)) * min_rgb
    S[I == 0] = 0  # If intensity is zero, saturation is zero

    # Calculate Hue
    num = 0.5 * ((R - G) + (R - B))
    den = np.sqrt((R - G)**2 + (R - B) * (G - B)) + 1e-6 # Avoid division by zero by adding 0.000001
    theta = np.arccos(num / den)
    H = np.zeros_like(I)

    H[B <= G] = theta[B <= G]
    H[B > G] = (2 * np.pi) - theta[B > G]
    H = H / (2 * np.pi)  # Normalize to [0, 1]

    HSI = cv2.merge((H, S, I))
    return HSI

for y, row in enumerate(median):
    for x, pixel in enumerate(row):
         if pixel == 255:
               # check surrounding pixels
               white_pixel_count = 0
               for j in range(-5, 6):
                  for i in range(-5, 6):
                     if y+j >= 0 and y+j < median.shape[0] and x+i >= 0 and x+i < median.shape[1]:
                           if median[y+j, x+i] > 200:
                              white_pixel_count += 1
                              if x < x_spot_lowest or x_spot_lowest == 0:
                                 x_spot_lowest = x
                              if x > x_spot_highest:
                                 x_spot_highest = x
                              if y < y_spot_lowest or y_spot_lowest == 0:
                                 y_spot_lowest = y
                              if y > y_spot_highest:
                                 y_spot_highest = y
    

                  #   for i in range(1, 10):
                  #      img[int(average_y)+i, int(average_x)+j] = [255, 0, 0]
                           
            ##print(f"value at {x} and {y} = {pixel}")
            #for j in range(1, 10):
            #    for i in range(1, 10):
            #        img[30+i, 100+j] = [255, 0, 0]
square_x = x_spot_highest - x_spot_lowest
square_y = y_spot_highest - y_spot_lowest
for j in range(square_y):
      for i in range(1,3):
         img[y_spot_lowest + j, x_spot_lowest+i] = [255, 0, 0]
         img[y_spot_lowest + j, x_spot_highest+i] = [255, 0, 0]
for i in range(square_x):
    for j in range(1,3):
      img[y_spot_lowest+j, x_spot_lowest + i] = [255, 0, 0]
      img[y_spot_highest+j, x_spot_lowest + i] = [255, 0, 0]

print(f"Lowest X: {x_spot_lowest}, Highest X: {x_spot_highest},s Lowest Y: {y_spot_lowest}, Highest Y: {y_spot_highest}")             

rezised_img = cv2.resize(img, (rezised_height, rezised_width))
rezised_img_color = cv2.cvtColor(rezised_img, cv2.COLOR_BGR2RGB)

#b,g,r = cv2.split(img)
#img[:,:,1] = 0

for y, row in enumerate(rezised_img):
    for x, pixel in enumerate(row):
         if np.all(pixel > [30, 70, 30]) and np.all(pixel < [130, 180, 130]):
               rezised_img[y, x] = [255, 255, 255]

def blur(img):
        median = cv2.medianBlur(img, 11) 
        return median
rezised_img = cv2.resize(blur(rezised_img), (1080, 720))
#cv2.imshow("Dildo1", rezised_img)
#cv2.imshow("Dildo", blur(rezised_img))


double_blur = blur(blur(resized_img2))
avereage = cv2.mean(double_blur)
print(avereage)
color = ('b','g','r')
#for i,col in enumerate(color):
#    histr = cv2.calcHist([img],[i],None,[256],[0,256])
#    plt.plot(histr,color = col)
#    plt.xlim([0,256])
#plt.show()
#plt.show()
#cv2.imshow("With square", (img))
#cv2.imshow("Original Image", (og_image))
#cv2.imshow("Just square", (img - og_image))
#cv2.imshow("With square", (blur(blur(blur(img)))))
cv2.imshow("blur one", double_blur)
cv2.imshow("with", resized_img2)
cv2.waitKey(0)


