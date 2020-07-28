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

PI=np.pi
def exact_sol(time, freq, X0, ml, w, h, g):
    sol=0
    for n in range(1,50):
        wn_2=(2*n-1) * PI* g / w * np.tanh( (2*n-1)*PI*h/w )
        sol=sol+ np.tanh( (2*n-1)*PI*h/w ) * np.power(2*PI*freq,2) / ( np.power( (2*n-1)*PI , 3 ) * (wn_2-np.power(2*PI*freq,2)) )


    return np.power(2*PI*freq,2)*X0*ml*(1+8*w/h*sol)*np.sin(2*PI*freq*time)




data_ISPH = pd.read_csv(str(sys.argv[1]))
data_WCSPH = pd.read_csv(str(sys.argv[2]))


PLOT_EVERY_N=1
PLOT_EVERY_N_isph=5
# First, design the Buterworth filter
N = 5    # Filter order
Wn = 0.9 # Cutoff frequency
B, A = signal.butter(N, Wn, output='ba')
fx_isph = signal.filtfilt(B, A, data_ISPH["Fx"])
# fx_wcsph = signal.filtfilt(B, A, data_WCSPH["Fx"])
fx_wcsph= data_WCSPH["Fx"]



major_ticks = np.arange(0, 101, 20)
minor_ticks = np.arange(0, 101, 5)

fig = plt.figure(num=None, figsize=(16, 9), dpi=60,
                 facecolor='w', edgecolor='k')
ax1 = fig.add_subplot(111)
ax1.set_title("Sloshing")
ax1.set_ylabel('F($N$)')
ax1.set_xlabel('time($s$)')
ax1.grid(color='k', linestyle='--', linewidth=0.1)
ax1.autoscale(enable=True, axis='x', tight=True)
ax1.plot(data_ISPH["Time"][::PLOT_EVERY_N_isph],fx_isph[::PLOT_EVERY_N_isph], 'b', label='ISPH', linewidth=2.0)
ax1.plot(data_WCSPH["Time"][::PLOT_EVERY_N],(fx_wcsph[::PLOT_EVERY_N]), 'r', label='WCSPH', linewidth=2.0)
# +11190*.124892*1.0
# ax1.plot(time,fy_smooth,  label='fy', linewidth=1.0)
# ax1.plot(time,fz_smooth,  label='fz', linewidth=1.0)

freq=0.75
X0=0.1
w=1.2
b=0.4
h=1.7
ml_1=0.124892*7560#*15096*
ml_2=h*w*b*1000



exact_solution=exact_sol(data_WCSPH["Time"], freq=freq, X0=X0, ml=ml_1, w = w,  h = h, g = 1.0)
ax1.plot(data_WCSPH["Time"],exact_solution, '--k', label='Exact', linewidth=2.0)


# ax2 = ax1.twinx()  # instantiate a second axes that shares the same x-axis
# ax2.plot(time,data["x"], 'b', label='x', linewidth=2.0)
leg = ax1.legend()
# leg = ax2.legend()

ax1.set_ylim(-5000, 5000)
ax1.set_xlim(0, 10)


outname = (str(sys.argv[1]).replace("/FS_body0.csv", ""))[-6:]
plt.savefig('%s.png' % outname)
plt.show()
