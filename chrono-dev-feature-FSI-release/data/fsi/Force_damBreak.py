#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""
Created on Mon Dec 17 20:57:10 2018

@author: milad
"""

import csv
import os
import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
from decimal import Decimal
from scipy.optimize import curve_fit

import scipy.signal as signal




matplotlib.rcParams['mathtext.fontset'] = 'stix'
matplotlib.rcParams['font.family'] = 'STIXGeneral'
matplotlib.rcParams.update({'font.size': 18})

time=[]
fx=[]
fy=[]
fz=[]

with open(str(sys.argv[1]),'r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    for row in plots:
        time.append(float(row[0]))
        fx.append(float(row[1]))
        fy.append(float(row[2]))
        fz.append(float(row[3]))



time=np.array(time);
fx=np.array(fx);
fy=np.array(fy);
fz=np.array(fz);



# First, design the Buterworth filter
N  = 8    # Filter order
Wn = 0.05 # Cutoff frequency
B, A = signal.butter(N, Wn, output='ba')
fx_smooth = signal.filtfilt(B,A, fx)
fy_smooth = signal.filtfilt(B,A, fy)
fz_smooth = signal.filtfilt(B,A, fz)


major_ticks = np.arange(0, 101, 20)
minor_ticks = np.arange(0, 101, 5)

fig = plt.figure(num=None, figsize=(8, 6), dpi=140, facecolor='w', edgecolor='k')
ax1 = fig.add_subplot(111)
ax1.set_title("dam break with Obstacle")    
ax1.set_ylabel('F($N$)')
ax1.set_xlabel('time($s$)')
ax1.grid(color='k', linestyle='-', linewidth=0.1)
ax1.autoscale(enable=True, axis='x', tight=True)
ax1.plot(time,fx,'r', label='fx',linewidth=0.1)
ax1.plot(time,fz, 'b', label='fy', linewidth=0.1)
ax1.plot(time,fz, 'k', label='fz', linewidth=0.1)

#ax1.plot(time,fx_smooth,'r', label='fx_smooth', linewidth=0.5)
#ax1.plot(time,fy_smooth,'b', label='fy_smooth', linewidth=0.5)
#ax1.plot(time,fz_smooth, 'k', label='fz_smooth', linewidth=0.5)

leg = ax1.legend()
#ax1.set_xlim(0,5)

plt.show()
