import sys
import numpy as np
import matplotlib
matplotlib.use('TkAgg')
# matplotlib.use('Agg')  # Must be before importing matplotlib.pyplot or pylab!
import matplotlib.pyplot as plt


from decimal import Decimal
from scipy.optimize import curve_fit
import pandas as pd
import scipy.signal as signal




matplotlib.rcParams['mathtext.fontset'] = 'stix'
matplotlib.rcParams['font.family'] = 'STIXGeneral'
matplotlib.rcParams.update({'font.size': 18})

path = str(sys.argv[1])
fileName = path + ("/FS_body0.csv")
data=pd.read_csv(fileName)

time=data["Time"]
x=data["x"]
y=data["y"]
z=data["z"]
vx=data["Vx"]
vy=data["Vy"]
vz=data["Vz"]


# First, design the Buterworth filter
N  = 5    # Filter order
Wn = 0.99 # Cutoff frequency
B, A = signal.butter(N, Wn, output='ba')
fx_smooth = signal.filtfilt(B,A, data["Fx"])
fy_smooth = signal.filtfilt(B,A, data["Fy"])
fz_smooth = signal.filtfilt(B,A, data["Fz"])
Tx_smooth = signal.filtfilt(B,A, data["Tx"])
Ty_smooth = signal.filtfilt(B,A, data["Ty"])
Tz_smooth = signal.filtfilt(B,A, data["Tz"])


major_ticks = np.arange(0, 101, 20)
minor_ticks = np.arange(0, 101, 5)

fig = plt.figure(num=None, figsize=(8, 6), dpi=140, facecolor='w', edgecolor='k')
ax1 = fig.add_subplot(111)
#ax1.set_title("Flow over cylinder")
ax1.set_ylabel('F($N$)')
ax1.set_xlabel('time($s$)')
ax1.grid(color='k', linestyle='-', linewidth=0.1)
ax1.autoscale(enable=True, axis='x', tight=True)
#ax1.plot(time,fx,'r', label='fx',linewidth=0.1)
ax1.plot(time,fx_smooth,'g', label='fx', linewidth=1)
ax1.plot(time,fy_smooth,'y', label='fy', linewidth=1)
ax1.plot(time,fz_smooth,'c', label='fz', linewidth=1)

ax1.plot(time,Tx_smooth, 'k', label='Tx', linewidth=1)
ax1.plot(time,Ty_smooth, 'b', label='Ty', linewidth=1)
ax1.plot(time,Tz_smooth, 'r', label='Tz', linewidth=1)

# ax1.plot(time,x,'c', label='x', linewidth=1)
# ax1.plot(time,z,'y', label='z', linewidth=1)


leg = ax1.legend()
plt.show()
