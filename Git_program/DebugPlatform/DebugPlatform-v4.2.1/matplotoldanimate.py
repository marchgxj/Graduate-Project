__author__ = 'Changxiaodong'
# coding=utf-8
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import Tkinter as Tk
import matplotlib
matplotlib.use('TkAgg')
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.backend_bases import key_press_handler
import ttk


class Scope:
    def __init__(self, data, thread):
        self.root = Tk.Tk()
        self.root.wm_title("2D Image")
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
        self.ResultLine, = self.GMIsensorax.plot([], [], "b", lw=2)

        self.count = 0

        for v in data[1:]:
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
        xlim_l = self.xlim - (self.therad.datascalevalue * -20 + 220)
        xlim_r = self.xlim + (self.therad.datascalevalue * -20 + 220)

        self.sensorax.set_xlim(xlim_l,xlim_r )
        self.Varianceax.set_xlim(xlim_l, xlim_r)
        self.Extremumax.set_xlim(xlim_l, xlim_r)
        self.Stateax.set_xlim(xlim_l, xlim_r)
        self.Intensityax.set_xlim(xlim_l, xlim_r)
        self.GMIsensorax.set_xlim(xlim_l, xlim_r)
        return
        # return self.XValueLine, self.YValueLine, self.VarValueLine, self.ExtValueLine, self.VarStateLine, self.ExtStateLine, self.IntensityLine, self.IntensityMiddleLine, self.IntStateLine,self.ResultLine

    def setScaleValue(self):
        self.datascale.set(self.therad.x1/self.therad.datascalevalue)
        self.datascalelabel.config(text=str(self.datascale.get()))

    def setScalueTo(self):
        self.datascale.config(to=len(self.XValueLinedata)*self.therad.datascalevalue)

    def start(self):

        canvas = FigureCanvasTkAgg(self.fig, master=self.root)
        canvas.show()
        canvas.get_tk_widget().pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)


        ani = animation.FuncAnimation(self.fig, self.update, init_func=self.init, frames=50, interval=200)
        # plt.show()

        toolbar = NavigationToolbar2TkAgg(canvas, self.root)
        toolbar.update()
        canvas._tkcanvas.pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)


        def _quit():
            self.root.quit()     # stops mainloop
            self.root.destroy()  # this is necessary on Windows to prevent
                            # Fatal Python Error: PyEval_RestoreThread: NULL tstate

        self.datascale = ttk.Scale(self.root, orient=Tk.HORIZONTAL, from_=1, to=len(self.XValueLinedata)*self.therad.datascalevalue,command=self.scaleCallback)
        self.datascale.pack(fil=Tk.X)
        self.datascalevalue = 5
        self.datascale.set(self.datascalevalue/self.therad.datascalevalue)
        self.datascalelabel = ttk.Label(self.root, text="5")
        self.datascalelabel.pack()

        button = Tk.Button(master=self.root, text='Quit', command=_quit)
        button.pack(side=Tk.BOTTOM)

        Tk.mainloop()

    def scaleCallback(self,event):

        datascalevalue = int(self.datascale.get())
        try:
            self.datascalelabel.config(text=str(datascalevalue/self.therad.datascalevalue))
        except:
            pass

        self.therad.Showdetaildata(datascalevalue)
        self.xlim = datascalevalue

        self.sensorax.set_xlim(self.xlim - 50, self.xlim + 50)
        self.Varianceax.set_xlim(self.xlim - 50, self.xlim + 50)
        self.Extremumax.set_xlim(self.xlim - 50, self.xlim + 50)
        self.Stateax.set_xlim(self.xlim - 50, self.xlim + 50)
        self.Intensityax.set_xlim(self.xlim - 50, self.xlim + 50)
        self.GMIsensorax.set_xlim(self.xlim - 50, self.xlim + 50)


    def closeing(self):
        plt.close()


if __name__ == '__main__':
    scope = Scope()
    scope.start()
