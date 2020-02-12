#!/usr/bin/env python
# coding: utf-8

# In[174]:


import matplotlib.pyplot as plt
import numpy as np
import csv
import simplekml
from glob import glob
from colour import Color

time = []
x = []
y = []
z = []
velocity = []
kml = simplekml.Kml()
filename = glob('*.csv')[0]
my_data = np.genfromtxt(filename, delimiter=";", skip_header=True)
flag = False
previousCords = [0, 0]
minZ = my_data.min(axis=0)[3] * 1.0
maxZ = my_data.max(axis=0)[3] * 1.0
colors = list(Color('green').range_to(Color('blue'), 10))
#print(minZ)
#print(maxZ)

def pick_color(m_min, m_max, m_x):
    return int(((m_x - m_min)/(m_max - m_min)) * 9)


with open(filename, 'r') as csvfile:
    plots = csv.reader(csvfile, delimiter=';')
    for row in plots:
        time.append(np.datetime64(row[0]))
        x.append(float(row[1]))
        y.append(float(row[2]))
        z.append(float(row[3]))
        velocity.append(float(row[6]))
        kml.newpoint(coords=[(row[5], row[4])], description="speed = " + row[6] + " km/h")
        if flag is True:
            ls = kml.newlinestring(name='A LineString')
            ls.coords = [(previousCords[0], previousCords[1]), (row[5], row[4])]
            ls.style.linestyle.width = 15
            #print(pick_color(minZ, maxZ, int(row[3])))
            ls.style.linestyle.color = colors[pick_color(minZ, maxZ, int(row[3]))].hex_l.replace('#', 'ff')
        previousCords[0] = row[5]
        previousCords[1] = row[4]
        flag = True

fig = plt.figure(figsize=(30, 15))

plt.subplot(4, 1, 1)
plt.plot(time, x, color='red', marker='o', label='X axis', linewidth=2, markersize=5)
plt.ylabel('Values', size=25)
plt.legend(fontsize='xx-large')
plt.title('Acceleration', size=25)

plt.subplot(4, 1, 2)
plt.plot(time, y, color='green', marker='o', label='Y axis', linewidth=2, markersize=5)
plt.ylabel('Values', size=25)
plt.legend(fontsize='xx-large')

plt.subplot(4, 1, 3)
plt.plot(time, z, color='blue', marker='o', label='Z axis', linewidth=2, markersize=5)
plt.ylabel('Values', size=25)
plt.legend(fontsize='xx-large')

plt.subplot(4, 1, 4)
plt.plot(time, velocity, label='velocity', marker='o', linewidth=2, markersize=5)
plt.xlabel('Time', size=25)
plt.ylabel('Values', size=25)
plt.legend(fontsize='xx-large')
plt.title('Speed in km/h', size=25)

plt.savefig('DriveStyle.png', dpi=None, facecolor='w', edgecolor='w',
            orientation='portrait', papertype=None, format='png',
            transparent=False, bbox_inches=None, pad_inches=0.1)
kml.save('DriveStyle.kml')

# In[ ]:
