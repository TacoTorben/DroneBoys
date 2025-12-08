import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import cv2
import numpy as np
df = pd.read_csv("/home/dksoren/DroneBoys/Images/dataFinal.csv", sep=";")
#df2 = pd.read_csv("/home/dksoren/DroneBoys/Images/JesperData.csv", sep=",")
#print(df.columns)


#plt.style.use('_mpl-gallery')
#
#
#plt.scatter(df["Area"], df["Perimeter"], c="blue", marker="o", s=10)
#plt.xlabel("Area")
#plt.ylabel("Perimeter")
#plt.show()
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
x1 = df["Area"]
y1 = df["Perimeter"]
z1 = df["Circularity"]

# Second dataset
#x2 = df2["Area"]
#y2 = df2["Perimeter"]
#z2 = df2["Circularity"]

ax.scatter(x1, y1, z1, s=40, color="red", label="False positives")
#ax.scatter(x2, y2, z2, s=40, color="blue", label="Jesper's blobs")

ax.set_xlabel("Area")
ax.set_ylabel("Perimeter")
ax.set_zlabel("Circularity")
plt.legend()
plt.show()


#aspe_ratio not showing much variance
