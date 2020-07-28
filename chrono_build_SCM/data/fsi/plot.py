import csv
import os
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
from decimal import Decimal

from scipy.optimize import curve_fit

matplotlib.rcParams['mathtext.fontset'] = 'stix'
matplotlib.rcParams['font.family'] = 'STIXGeneral'
#matplotlib.rcParams.update({'font.size': 24})
plt.rc('xtick', labelsize=20) 
plt.rc('ytick', labelsize=20) 

x_CFD=[]
with open('./front_pos.txt','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=' ')
    for row in plots:
        x_CFD.append(float(row[0]))
    


x_CFD=np.array(x_CFD)-x_CFD[0]; #x
t_CFD=np.arange(x_CFD.shape[0])*0.05

x_DEM=[]
with open('./DEM.txt','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=' ')
    for row in plots:
        x_DEM.append(float(row[0]))
    


x_DEM=np.array(x_DEM)-x_DEM[0]; #x
t_DEM=np.arange(x_DEM.shape[0])*0.01




major_ticks = np.arange(0, 101, 20)
minor_ticks = np.arange(0, 101, 5)




fig = plt.figure(num=None, figsize=(8, 6), dpi=140, facecolor='w', edgecolor='k')
ax1 = fig.add_subplot(111)
ax1.set_title("Front position", fontsize=26)    
ax1.set_ylabel('x($m$)', fontsize=24)
ax1.set_xlabel('time($s$)', fontsize=24)

ax1.grid(color='k', linestyle='-', linewidth=0.2)
ax1.autoscale(enable=True, axis='x', tight=True)
ax1.plot(t_CFD,x_CFD, 'k', label='CFD')
ax1.plot(t_DEM,x_DEM*0.01, 'b', label='DEM')
leg = ax1.legend()
ax1.set_xlim(0, 1.0)


plt.show()
