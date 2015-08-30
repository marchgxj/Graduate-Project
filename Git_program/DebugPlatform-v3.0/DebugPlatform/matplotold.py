# coding=utf-8
import datetime
import matplotlib.pyplot as plt
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
        for v in data:
            self.XValue.append(v[0])
            self.YValue.append(v[1])
            self.VarValue.append(v[2])
            self.MiddleValue.append(v[3])
            self.ExtValue.append(v[4])
        # print self.XValue
        # print self.YValue
        # print self.VarValue
        # print self.MiddleValue
        # print self.ExtValue
        # date1 = datetime.datetime( int(time.strftime('%Y', time.localtime(float(datatime[0])))),int(time.strftime('%m', time.localtime(float(datatime[0])))),int(time.strftime('%d', time.localtime(float(datatime[0])))),
        #                            int(time.strftime('%H', time.localtime(float(datatime[0])))),int(time.strftime('%M', time.localtime(float(datatime[0])))),int(time.strftime('%S', time.localtime(float(datatime[0])))))
        # date2 = datetime.datetime( int(time.strftime('%Y', time.localtime(float(datatime[-1])))),int(time.strftime('%m', time.localtime(float(datatime[-1])))),int(time.strftime('%d', time.localtime(float(datatime[-1])))),
        #                            int(time.strftime('%H', time.localtime(float(datatime[-1])))),int(time.strftime('%M', time.localtime(float(datatime[-1])))),int(time.strftime('%S', time.localtime(float(datatime[-1])))))
        # delta = datetime.timedelta(microseconds = 100000)
        # self.dates = drange(date1, date2, delta)

    def show(self):
        self.fig = plt.figure(figsize=(12, 6), dpi=100)
        self.sensorax = self.fig.add_subplot(2, 2, 1, xlim=(0, 10), ylim=(0, 4096))
        self.Varianceax = self.fig.add_subplot(2, 2, 3, xlim=(0, 10), ylim=(0, 40000))
        self.Extremumax = self.fig.add_subplot(2, 2, 4, xlim=(0, 10), ylim=(0, 10000))
        self.backupax = self.fig.add_subplot(2, 2, 2, xlim=(0, 10), ylim=(0, 10000))


        self.sensorax.plot(range(len(self.XValue)),self.XValue)
        self.sensorax.plot(range(len(self.YValue)),self.YValue)
        self.sensorax.set_xlim(0,len(self.XValue))

        self.Varianceax.plot(range(len(self.VarValue)),self.VarValue)
        self.Varianceax.set_xlim(0,len(self.VarValue))

        self.Extremumax.plot(range(len(self.ExtValue)),self.ExtValue)
        self.Extremumax.set_xlim(0,len(self.ExtValue))
        # if(len(self.dates)>len(self.XValue)):
        #     self.sensorax.plot_date(self.dates[:len(self.XValue)],self.XValue)
        # else:
        #     self.sensorax.plot_date(self.dates,self.XValue[:len(self.dates)])
        # self.sensorax.set_xlim(self.dates[0], self.dates[-1] )# ax.xaxis.set_major_locator( DayLocator() )
        # self.sensorax.xaxis.set_minor_locator( HourLocator(arange(0,25,6)) )
        # self.sensorax.xaxis.set_major_formatter( DateFormatter('%H:%M:%S') )
        # self.sensorax.fmt_xdata = DateFormatter('%Y-%m-%d %H:%M:%S')
        # self.fig.autofmt_xdate()
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

