import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
from decimal import Decimal
from scipy.optimize import curve_fit
import pandas as pd

import scipy.signal as signal


matplotlib.rcParams['mathtext.fontset'] = 'stix'
matplotlib.rcParams['font.family'] = 'STIXGeneral'
matplotlib.rcParams.update({'font.size': 18})

invfps = 1./20
sampleAll = True


data = pd.read_csv(str(sys.argv[1]))
time = data["Time"]
x = data["x"]
y = data["y"]
z = data["z"]
vx = data["Vx"]
vy = data["Vy"]
vz = data["Vz"]


# First, design the Buterworth filter
N = 5    # Filter order
Wn = 0.01  # Cutoff frequency
B, A = signal.butter(N, Wn, output='ba')
fx_smooth = signal.filtfilt(B, A, data["Fx"])
fy_smooth = signal.filtfilt(B, A, data["Fy"])
fz_smooth = signal.filtfilt(B, A, data["Fz"])
Tx_smooth = signal.filtfilt(B, A, data["Tx"])
Ty_smooth = signal.filtfilt(B, A, data["Ty"])
Tz_smooth = signal.filtfilt(B, A, data["Tz"])


major_ticks = np.arange(0, 101, 20)
minor_ticks = np.arange(0, 101, 5)

fig = plt.figure(num=None, figsize=(16, 9), dpi=60,
                 facecolor='w', edgecolor='k')
ax1 = fig.add_subplot(111)
ax1.set_title("Dam break with cylinder")
ax1.set_ylabel('F($N$)')
ax1.set_xlabel('time($s$)')
ax1.grid(color='k', linestyle='--', linewidth=0.1)
ax1.autoscale(enable=True, axis='x', tight=True)
#ax1.plot(time,fx,'r', label='fx',linewidth=0.1)
ax1.plot(time, fx_smooth, 'r', label='fx_', linewidth=1.0)
#ax1.plot(time,fz, 'k', label='fz', linewidth=0.1)
#ax1.plot(time,fz_smooth, 'k', label='fz_', linewidth=0.5)

leg = ax1.legend()
#ax1.set_ylim(-0.01, 0.02)

outname = (str(sys.argv[1]).replace("/FS_body0.csv", ""))[-6:]
plt.savefig('%s.png' % outname)
# plt.show()
