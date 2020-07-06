import csv,os,sys
import numpy as np
import pandas as pd
import matplotlib
matplotlib.use('TkAgg')
# matplotlib.use('Agg')  # Must be before importing matplotlib.pyplot or pylab!
import matplotlib.pyplot as plt
from decimal import Decimal
from scipy.optimize import curve_fit
import scipy.signal as signal


matplotlib.rcParams['mathtext.fontset'] = 'stix'
matplotlib.rcParams['font.family'] = 'STIXGeneral'
matplotlib.rcParams.update({'font.size': 24})


path = str(sys.argv[1])
numBody = str(sys.argv[2])

for b in range(0, int(numBody)):
    fileName = path + ("/FS_body%d.csv" % b)
    data=pd.read_csv(fileName)
    if(b == 0):
        time=data["Time"]
        x=data["x"]
        y=data["y"]
        z=data["z"]
        vx=data["Vx"]
        vy=data["Vy"]
        vz=data["Vz"]
        fx=data["Fx"]
        fy=data["Fy"]
        fz=data["Fz"]
        Tx=data["Tx"]
        Ty=data["Ty"]
        Tz=data["Tz"]

    else:
        time = np.vstack((time, np.array(data["Time"], ndmin=2)))
        x = np.vstack((x, np.array(data["x"], ndmin=2)))
        y = np.vstack((y, np.array(data["y"], ndmin=2)))
        z = np.vstack((z, np.array(data["z"], ndmin=2)))
        vx = np.vstack((x, np.array(data["Vx"], ndmin=2)))
        vy = np.vstack((y, np.array(data["Vy"], ndmin=2)))
        vz = np.vstack((z, np.array(data["Vz"], ndmin=2)))
        fx = np.vstack((fx, np.array(data["Fx"], ndmin=2)))
        fy = np.vstack((fy, np.array(data["Fy"], ndmin=2)))
        fz = np.vstack((fz, np.array(data["Fz"], ndmin=2)))
        Tx = np.vstack((Tx, np.array(data["Tx"], ndmin=2)))
        Ty = np.vstack((Ty, np.array(data["Ty"], ndmin=2)))
        Tz = np.vstack((Tz, np.array(data["Tz"], ndmin=2)))



print(x.shape, fx.shape, Tx.shape)
# First, design the Buterworth filter
N = 5    # Filter order
Wn = 0.95  # Cutoff frequency
B, A = signal.butter(N, Wn, output='ba')
fx_smooth = signal.filtfilt(B, A, fx)
fy_smooth = signal.filtfilt(B, A, fy)
fz_smooth = signal.filtfilt(B, A, fz)
Tx_smooth = signal.filtfilt(B, A, Tx)
Ty_smooth = signal.filtfilt(B, A, Ty)
Tz_smooth = signal.filtfilt(B, A, Tz)


major_ticks = np.arange(0, 101, 20)
minor_ticks = np.arange(0, 101, 5)
fig = plt.figure(num=None, figsize=(12, 10), dpi=80,
                 facecolor='w', edgecolor='k')

ax1 = fig.add_subplot(411)
ax2 = fig.add_subplot(412)
ax3 = fig.add_subplot(413)
ax4 = fig.add_subplot(414)
# ax1.set(xlim=[0, 10],ylim=[-1,1])
# ax2.set(xlim=[0, 10],ylim=[-1,1])
# ax3.set(xlim=[0, 10],ylim=[-1,1])
# ax4.set(xlim=[0, 10],ylim=[-1,1])


ax4.grid(True)
# ax1.set_title("Fish")
# ax1.set_ylabel('F($N$)')
# ax1.set_xlabel('time($s$)')
# ax1.grid(color='k', linestyle='-', linewidth=0.1)
# ax1.autoscale(enable=True, axis='x', tight=True)
#ax1.plot(time,fx,'r', label='fx',linewidth=0.1)

# for b in range(0,int(numBody)):
#     ax1.plot(time[b,:],fx_smooth[b,:], label='fx_%s'%b, linewidth=1.0)
#     ax2.plot(time[b,:],fy_smooth[b,:], label='fy_%s'%b, linewidth=1.0)
#     ax3.plot(time[b,:],fz_smooth[b,:], label='fz_%s'%b, linewidth=1.0)

ax1.plot(time[1, :], np.sum(fx_smooth, axis=0), label='fx_net', linewidth=1.0)
ax2.plot(time[1, :], np.sum(fy_smooth, axis=0), label='fy_net', linewidth=1.0)
ax3.plot(time[1, :], np.sum(fz_smooth, axis=0), label='fz_net', linewidth=1.0)
ax1.plot(time[1, :], np.sum(Tx_smooth, axis=0), label='Tx_net', linewidth=1.0)
ax2.plot(time[1, :], np.sum(Ty_smooth, axis=0), label='Ty_net', linewidth=1.0)
ax3.plot(time[1, :], np.sum(Tz_smooth, axis=0), label='Tz_net', linewidth=1.0)
ax4.plot(time[1, :], np.sum(vx, axis=0), label='Vx_net', linewidth=1.0)
ax4.plot(time[1, :], np.sum(vy, axis=0), label='Vy_net', linewidth=1.0)
ax4.plot(time[1, :], np.sum(vz, axis=0), label='Vz_net', linewidth=1.0)
ax4.plot(time[1, :], np.sum(x, axis=0), label='x', linewidth=1.0)
#print(np.sum(vz, axis=0))
# ax4.plot(np.mean(x,axis=0),np.mean(y,axis=0), label='x-y', linewidth=1.0)
fig.subplots_adjust(hspace=0.3)

#ax1.plot(time,fz, 'k', label='fz', linewidth=0.1)
#ax1.plot(time,fz_smooth, 'k', label='fz_', linewidth=0.5)

leg = ax1.legend()
leg = ax2.legend()
leg = ax3.legend()
leg = ax4.legend()

# ax1.set_ylim(-0,50)
# ax2.set_ylim(-00,50)
# ax3.set_ylim(-0,50)

outname = str(sys.argv[1])[-13:-10]
plt.savefig('%s.png' % outname, dpi=300)
plt.show()
