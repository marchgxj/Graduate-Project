__author__ = 'Changxiaodong'
# coding=utf-8
import numpy as np
from matplotlib.lines import Line2D
import matplotlib.pyplot as plt
import matplotlib.animation as animation

import matplotlib.axes as axes
import time
from matplotlib.figure import Figure
import matplotlib.dates as mdates
import datetime


class Scope:
    def __init__(self, data, thread):
        self.fig = plt.figure(figsize=(12, 6), dpi=100)
        self.sensorax = self.fig.add_subplot(3, 2, 1, xlim=(0, 10), ylim=(0, 4096))
        self.Varianceax = self.fig.add_subplot(3, 2, 3, xlim=(0, 10), ylim=(0, 3000))
        self.Extremumax = self.fig.add_subplot(3, 2, 4, xlim=(0, 10), ylim=(0, 1000))
        self.Stateax = self.fig.add_subplot(3, 2, 2, xlim=(0, 10), ylim=(0, 12))
        self.Intensityax = self.fig.add_subplot(3, 2, 5, xlim=(0, 10), ylim=(0, 4000))
        self.GMIsensorax = self.fig.add_subplot(3, 2, 6, xlim=(0, 10), ylim=(0, 4096))

        self.therad  = thread
        self.XValueLinedata = []
        self.YValueLinedata = []
        self.ZValueLinedata = []
        self.GMI_YValueLinedata = []
        self.GMI_XValueLinedata = []
        self.VarValueLinedata = []

        self.ExtValueLinedata = []
        self.ExtStateLinedata = []
        self.VarStateLinedata = []
        self.IntensityLinedata = []
        self.IntensityMiddleLinedata = []
        self.IntStateLinedata = []
        self.ResultLinedata = []
        self.xdata = []

        self.XValueLine, = self.sensorax.plot([], [], "b", lw=2)
        self.YValueLine, = self.sensorax.plot([], [], "g", lw=2)
        self.ZValueLine, = self.sensorax.plot([], [], "r", lw=2)
        self.GMI_XValueLine, = self.GMIsensorax.plot([], [], "b", lw=2)
        self.GMI_YValueLine, = self.GMIsensorax.plot([], [], "g", lw=2)
        self.VarValueLine, = self.Varianceax.plot([], [], lw=2)

        self.ExtValueLine, = self.Extremumax.plot([], [], "r", lw=2)
        self.ExtStateLine, = self.Stateax.plot([], [], "r", lw=2)
        self.VarStateLine, = self.Stateax.plot([], [], lw=2)
        self.IntensityLine, = self.Intensityax.plot([], [], "y", lw=2)
        self.IntensityMiddleLine, = self.Intensityax.plot([], [], "r", lw=2)
        self.IntStateLine, = self.Stateax.plot([], [], "y", lw=2)
        self.ResultLine, = self.Resultax.plot([], [], "b", lw=2)

        self.count = 0

        for v in data:
            self.XValueLinedata.append(v[0])
            self.YValueLinedata.append(v[1])
            self.ZValueLinedata.append(v[16])
            self.GMI_XValueLinedata.append(v[17])
            self.GMI_YValueLinedata.append(v[18])
            self.VarValueLinedata.append(v[2])
            self.ExtValueLinedata.append(v[4])
            if(int(v[5])==2):
                self.VarStateLinedata.append(1)
            elif(int(v[5])==1):
                self.VarStateLinedata.append(2)
            else:
                self.VarStateLinedata.append(0)
            if(int(v[6])==2):
                self.ExtStateLinedata.append(4)
            elif(int(v[6])==1):
                self.ExtStateLinedata.append(5)
            else:
                self.ExtStateLinedata.append(3)
            self.IntensityLinedata.append(v[9])
            self.IntensityMiddleLinedata.append(v[10])
            if(int(v[11])==2):
                self.IntStateLinedata.append(7)
            elif(int(v[11])==1):
                self.IntStateLinedata.append(8)
            else:
                self.IntStateLinedata.append(6)
            if(int(v[12])==1):
                self.ResultLinedata.append(11)
            else:
                self.ResultLinedata.append(9)


    def init(self):
        for i in range(len(self.XValueLinedata)):
            self.xdata.append(i)
        self.XValueLine.set_data(self.xdata, self.XValueLinedata)
        self.YValueLine.set_data(self.xdata, self.YValueLinedata)
        self.ZValueLine.set_data(self.xdata, self.ZValueLinedata)
        self.GMI_XValueLine.set_data(self.xdata, self.GMI_XValueLinedata)
        self.GMI_YValueLine.set_data(self.xdata, self.GMI_YValueLinedata)
        self.VarValueLine.set_data(self.xdata, self.VarValueLinedata)

        self.ExtValueLine.set_data(self.xdata, self.ExtValueLinedata)
        self.ExtStateLine.set_data(self.xdata, self.ExtStateLinedata)
        self.VarStateLine.set_data(self.xdata, self.VarStateLinedata)
        self.IntensityLine.set_data(self.xdata, self.IntensityLinedata)
        self.IntensityMiddleLine.set_data(self.xdata, self.IntensityMiddleLinedata)
        self.IntStateLine.set_data(self.xdata, self.IntStateLinedata)
        self.ResultLine.set_data(self.xdata, self.ResultLinedata)
        return self.XValueLine, self.YValueLine, self.VarValueLine, self.ExtValueLine, self.ExtStateLine, self.VarStateLine, self.IntensityLine, self.IntensityMiddleLine, self.IntStateLine,self.ResultLine

    def update(self, i):
        self.xlim = self.therad.xlim
        self.sensorax.set_xlim(self.xlim - 50, self.xlim + 50)
        self.Varianceax.set_xlim(self.xlim - 50, self.xlim + 50)
        self.Extremumax.set_xlim(self.xlim - 50, self.xlim + 50)
        self.Stateax.set_xlim(self.xlim - 50, self.xlim + 50)
        self.Intensityax.set_xlim(self.xlim - 50, self.xlim + 50)
        self.GMIsensorax.set_xlim(self.xlim - 50, self.xlim + 50)
        return
        # return self.XValueLine, self.YValueLine, self.VarValueLine, self.ExtValueLine, self.VarStateLine, self.ExtStateLine, self.IntensityLine, self.IntensityMiddleLine, self.IntStateLine,self.ResultLine

    def start(self):
        ani = animation.FuncAnimation(self.fig, self.update, init_func=self.init, frames=50, interval=200)
        plt.show()


    def closeing(self):
        plt.close()


if __name__ == '__main__':
    scope = Scope()
    scope.start()
