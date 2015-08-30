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
    def __init__(self, thread):
        self.fig = plt.figure(figsize=(12, 6), dpi=100)
        self.sensorax = self.fig.add_subplot(2, 2, 1, xlim=(0, 10), ylim=(0, 4096))
        self.backupax = self.fig.add_subplot(2, 2, 2, xlim=(0, 10), ylim=(0, 10000))
        self.Varianceax = self.fig.add_subplot(2, 2, 3, xlim=(0, 10), ylim=(0, 40000))
        self.Extremumax = self.fig.add_subplot(2, 2, 4, xlim=(0, 10), ylim=(0, 3000))
        self.thread = thread

        self.XValueLinedata = []
        self.YValueLinedata = []
        self.VarValueLinedata = []
        self.MiddleValueLinedata = []
        self.ExtValueLinedata = []
        self.xdata = []

        self.XValueLine, = self.sensorax.plot([], [], lw=2)
        self.YValueLine, = self.sensorax.plot([], [], lw=2)
        self.VarValueLine, = self.Varianceax.plot([], [], lw=2)
        self.MiddleValueLine, = self.Varianceax.plot([], [], lw=2)
        self.ExtValueLine, = self.Extremumax.plot([], [], lw=2)
        self.count = 0
    def init(self):
        self.XValueLine.set_data([], [])
        self.YValueLine.set_data([], [])
        self.VarValueLine.set_data([], [])
        self.MiddleValueLine.set_data([], [])
        self.ExtValueLine.set_data([], [])
        return self.XValueLine, self.YValueLine, self.VarValueLine, self.MiddleValueLine, self.ExtValueLine

    def update(self, i):
        self.count += 1
        self.sensorax.set_xlim(self.count - 50, self.count + 50)
        self.Varianceax.set_xlim(self.count - 50, self.count + 50)
        self.Extremumax.set_xlim(self.count - 50, self.count + 50)
        if (len(self.thread.value) != 0):
            self.xdata.append(self.count)
            self.XValueLinedata.append(self.thread.value[0])
            self.YValueLinedata.append(self.thread.value[1])
            self.VarValueLinedata.append(self.thread.value[2])
            self.MiddleValueLinedata.append(self.thread.value[3])
            self.ExtValueLinedata.append(self.thread.value[4])
        self.XValueLine.set_data(self.xdata, self.XValueLinedata)
        self.YValueLine.set_data(self.xdata, self.YValueLinedata)
        self.VarValueLine.set_data(self.xdata, self.VarValueLinedata)
        self.MiddleValueLine.set_data(self.xdata, self.MiddleValueLinedata)
        self.ExtValueLine.set_data(self.xdata, self.ExtValueLinedata)
        return self.XValueLine, self.YValueLine,self.VarValueLine,self.MiddleValueLine,self.ExtValueLine


    def start(self):
        ani = animation.FuncAnimation(self.fig, self.update, init_func=self.init, frames=50, interval=100)

        plt.show()


    def __del__(self):
        print "delete"


    def closeing(self):
        plt.close()


if __name__ == '__main__':
    scope = Scope()
    scope.start()
