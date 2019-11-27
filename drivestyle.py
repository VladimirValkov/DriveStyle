#!/usr/bin/env python
# coding: utf-8

# In[174]:


import matplotlib.pyplot as plt
import numpy as np
import csv

time = []
x = []
y = []
z = []
velocity = []

with open('test.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=';')
    for row in plots:
        time.append(np.datetime64(row[0]))
        x.append(float(row[1]))
        y.append(float(row[2]))
        z.append(float(row[3]))
        velocity.append(float(row[6]))
        
fig = plt.figure(figsize=(30,15))

plt.subplot(4,1,1)
plt.plot(time, x,color = 'red',marker='o' ,label='X axis',linewidth=2, markersize=5)
plt.ylabel('Values', size = 25)
plt.legend(fontsize = 'xx-large')
plt.title('Acceleration', size = 25)

plt.subplot(4,1,2)
plt.plot(time, y,color = 'green',marker='o' ,label='Y axis',linewidth=2, markersize=5)
plt.ylabel('Values', size = 25)
plt.legend(fontsize = 'xx-large')

plt.subplot(4,1,3)
plt.plot(time, z,color = 'blue',marker='o' ,label='Z axis',linewidth=2, markersize=5)
plt.ylabel('Values', size = 25)
plt.legend(fontsize = 'xx-large')

plt.subplot(4,1,4)
plt.plot(time, velocity, label='velocity',marker= 'o',linewidth=2, markersize=5)
plt.xlabel('Time', size = 25)
plt.ylabel('Values', size = 25)
plt.legend(fontsize = 'xx-large')
plt.title('Speed in km/h', size = 25)
plt.show()


# In[ ]:




