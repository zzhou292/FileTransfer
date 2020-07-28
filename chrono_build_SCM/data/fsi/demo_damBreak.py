import csv
import os
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
from decimal import Decimal

from scipy.optimize import curve_fit

def func(x, a):
    return a * np.power(-x,1.5)

def funcLinear(x, b):
    return b * x


matplotlib.rcParams['mathtext.fontset'] = 'stix'
matplotlib.rcParams['font.family'] = 'STIXGeneral'

Ex0=[]
Ex1=[]
with open('./ref_data/damBreakEx.txt','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=' ')
    for row in plots:
        Ex0.append(float(row[0]))
        Ex1.append(float(row[1]))

Ey0=[]
Ey1=[]
with open('./ref_data/damBreakEy.txt','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=' ')
    for row in plots:
        Ey0.append(float(row[0]))
        Ey1.append(float(row[1]))

c0=[]
c1=[]
c2=[]
with open('../../DEMO_OUTPUT/FSI_DAM_BREAK/Analysis.txt','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=' ')
    for row in plots:
        c0.append(float(row[0]))
        c1.append(float(row[1]))
        c2.append(float(row[2]))

tx_exp=np.array(Ex0); #time
x_exp=np.array(Ex1)-Ex1[0]; #x

ty_exp=np.array(Ey0); #time
y_exp=np.array(Ey1)-Ey1[0]; #y

t_num=np.array(c0); #time
x_num=np.array(c1)-c1[0]; #x
y_num=np.array(c2)-c2[0]; #y

#major_ticks = np.arange(0, 101, 20)
#minor_ticks = np.arange(0, 101, 5)
#



fig = plt.figure(num=None, figsize=(8, 6), dpi=140, facecolor='w', edgecolor='k')
ax1 = fig.add_subplot(211)
ax1.set_title("Dam break")
ax1.set_ylabel('x($mm$)')
ax1.grid(color='k', linestyle='-', linewidth=0.2)
ax1.autoscale(enable=True, axis='x', tight=True)
ax1.plot(tx_exp,x_exp, 'ko', label='Martin et al.')
ax1.plot(t_num,x_num, 'k--', label='Chrono FSI')

ax1.set_xticks(np.linspace(0, 4, 11))
ax1.set_yticks(np.linspace(0, 5, 11))
ax1.grid(which='both', linestyle='--', linewidth=0.5)
ax1.set_xlim(0, 4)
ax1.set_ylim(0, 5)
#ax1.set_yticks(np.linspace(0, 20, 21), minor=True)
ax1.grid(which='minor', alpha=0.2)
ax1.grid(which='major', alpha=0.5)



ax2 = fig.add_subplot(212)
ax2.grid(color='k', linestyle='-', linewidth=0.2)
ax2.set_ylabel('x($mm$)')
ax2.set_xlabel("time(s)")
ax2.autoscale(enable=True, axis='x', tight=True)
ax2.plot(t_num,y_num+1, 'k--', label='Chrono FSI')
ax2.plot(ty_exp,y_exp+1,'ko')


leg = ax1.legend()
ax2.set_xticks(np.linspace(0, 4, 11))
ax2.set_yticks(np.linspace(0, 1.2, 11))
ax2.grid(which='both', linestyle='--', linewidth=0.5)
ax2.set_xlim(0, 4)
ax2.set_ylim(0, 1.2)
ax2.set_yticks(np.linspace(0, 1.2, 21), minor=True)
ax2.grid(which='minor', alpha=0.2)
ax2.grid(which='major', alpha=0.5)
plt.show()
