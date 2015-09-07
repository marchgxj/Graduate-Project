# coding=utf-8
import datetime
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.dates import DayLocator, HourLocator, DateFormatter, drange
from numpy import arange
import tkMessageBox as tkmes
import time

class ScopeOld:
    def __init__(self,data):
        self.XValue=[]
        self.YValue=[]
        self.VarValue=[]
        self.MiddleValue=[]
        self.ExtValue=[]
        self.ExtState = []
        self.VarState = []
        self.Intensity = []
        self.IntensityMiddle = []
        self.IntState = []
        self.Result = []
        for v in data:
            self.XValue.append(v[0])
            self.YValue.append(v[1])
            self.VarValue.append(v[2])
            self.MiddleValue.append(v[3])
            self.ExtValue.append(v[4])
            if(int(v[5])==2):
                self.VarState.append(1)
            elif(int(v[5])==1):
                self.VarState.append(2)
            else:
                self.VarState.append(0)
            if(int(v[6])==2):
                self.ExtState.append(4)
            elif(int(v[6])==1):
                self.ExtState.append(5)
            else:
                self.ExtState.append(3)
            self.Intensity.append(v[9])
            self.IntensityMiddle.append(v[10])
            if(int(v[11])==2):
                self.IntState.append(7)
            elif(int(v[11])==1):
                self.IntState.append(8)
            else:
                self.IntState.append(6)
            self.Result.append(v[12])




    def show(self):
        self.fig = plt.figure(figsize=(12, 6), dpi=100)
        self.sensorax = self.fig.add_subplot(3, 2, 1, xlim=(0, 10), ylim=(0, 4096))
        self.Varianceax = self.fig.add_subplot(3, 2, 3, xlim=(0, 10), ylim=(0, 3000))
        self.Extremumax = self.fig.add_subplot(3, 2, 4, xlim=(0, 10), ylim=(0, 1000))
        self.Stateax = self.fig.add_subplot(3, 2, 2, xlim=(0, 10), ylim=(0, 9))
        self.Intensityax = self.fig.add_subplot(3, 2, 5, xlim=(0, 10), ylim=(0, 3000))
        self.Resultax = self.fig.add_subplot(3, 2, 6, xlim=(0, 10), ylim=(0, 4))

        self.sensorax.plot(range(len(self.XValue)),self.XValue)
        self.sensorax.plot(range(len(self.YValue)),self.YValue)
        self.sensorax.set_xlim(0,50)

        self.Varianceax.plot(range(len(self.VarValue)),self.VarValue)
        self.Varianceax.set_xlim(0,50)

        self.Extremumax.plot(range(len(self.ExtValue)),self.ExtValue,'r')
        self.Extremumax.set_xlim(0,50)

        self.Stateax.plot(range(len(self.VarState)),self.VarState,'b')
        self.Stateax.plot(range(len(self.ExtState)),self.ExtState,'r')
        self.Stateax.plot(range(len(self.IntState)),self.IntState,'y')
        self.Stateax.set_xlim(0,50)

        self.Intensityax.plot(range(len(self.Intensity)),self.Intensity,'y')
        self.Intensityax.plot(range(len(self.IntensityMiddle)),self.IntensityMiddle,'r')
        self.Intensityax.set_xlim(0,50)

        self.Resultax.plot(range(len(self.Result)),self.Result,'b')
        self.Resultax.set_xlim(0,50)



        plt.show()

    def set_xlim(self,x):
        x1 = x-50
        x2 = x+50
        self.sensorax.set_xlim(x1 - 50, x2 + 50)
        self.Varianceax.set_xlim(x1 - 50, x2 + 50)
        self.Extremumax.set_xlim(x1- 50, x2 + 50)
        self.Stateax.set_xlim(x1 - 50, x2 + 50)
        self.Intensityax.set_xlim(x1 - 50, x2 + 50)
        self.Resultax.set_xlim(x1 - 50, x2 + 50)
        plt.show()










# y = arange( len(dates)*1.0)
#
# fig, ax = plt.subplots()
# ax.plot_date(dates, y*y)
#
# # this is superfluous, since the autoscaler should get it right, but
# # use date2num and num2date to convert between dates and floats if
# # you want; both date2num and num2date convert an instance or sequence
# ax.set_xlim( dates[0], dates[-1] )
#
# # The hour locator takes the hour or sequence of hours you want to
# # tick, not the base multiple
#
# ax.xaxis.set_major_locator( DayLocator() )
# ax.xaxis.set_minor_locator( HourLocator(arange(0,25,6)) )
# ax.xaxis.set_major_formatter( DateFormatter('%Y-%m-%d') )
#
# ax.fmt_xdata = DateFormatter('%Y-%m-%d %H:%M:%S')
# fig.autofmt_xdate()
#
# plt.show()

