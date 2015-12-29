# coding=utf-8

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import mpl_toolkits.mplot3d.axes3d as p3

import Tkinter as Tk


class Scope:
    def __init__(self, thread):
        self.fig = plt.figure(figsize=(12, 6), dpi=100)
        self.sensorax = self.fig.add_subplot(3, 2, 1, xlim=(0, 10), ylim=(0, 4096))
        self.Varianceax = self.fig.add_subplot(3, 2, 3, xlim=(0, 10), ylim=(0, 3000))
        self.Extremumax = self.fig.add_subplot(3, 2, 4, xlim=(0, 10), ylim=(0, 1000))
        self.Stateax = self.fig.add_subplot(3, 2, 2, xlim=(0, 10), ylim=(0, 12))
        self.Intensityax = self.fig.add_subplot(3, 2, 5, xlim=(0, 10), ylim=(0, 4000))
        self.GMIsensorax = self.fig.add_subplot(3, 2, 6, xlim=(0, 10), ylim=(0, 4096))

        self.thread = thread

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
        self.ResultLine, = self.Stateax.plot([], [], "g", lw=2)

        self.count = 0

    def init(self):
        self.XValueLine.set_data([], [])
        self.YValueLine.set_data([], [])
        self.ZValueLine.set_data([], [])
        self.GMI_XValueLine.set_data([], [])
        self.GMI_YValueLine.set_data([], [])
        self.VarValueLine.set_data([], [])

        self.ExtValueLine.set_data([], [])
        self.ExtStateLine.set_data([], [])
        self.VarStateLine.set_data([], [])
        self.IntensityLine.set_data([], [])
        self.IntensityMiddleLine.set_data([], [])
        self.IntStateLine.set_data([], [])
        self.ResultLine.set_data([], [])
        return self.XValueLine, self.YValueLine,self.ZValueLine,self.GMI_XValueLine,self.GMI_YValueLine, self.VarValueLine, self.ExtValueLine, self.ExtStateLine, self.VarStateLine, self.IntensityLine, self.IntensityMiddleLine, self.IntStateLine,self.ResultLine

    def update(self, i):
        self.count = self.thread.framecount
        self.sensorax.set_xlim(self.count - 50, self.count + 50)
        self.Varianceax.set_xlim(self.count - 50, self.count + 50)
        self.Extremumax.set_xlim(self.count - 50, self.count + 50)
        self.Stateax.set_xlim(self.count - 50, self.count + 50)
        self.Intensityax.set_xlim(self.count - 50, self.count + 50)
        self.GMIsensorax.set_xlim(self.count - 50, self.count + 50)

        if (len(self.thread.value) == 26):
            self.xdata.append(self.count)
            self.XValueLinedata.append(self.thread.value[0])
            self.YValueLinedata.append(self.thread.value[1])
            self.ZValueLinedata.append(self.thread.value[16])
            self.GMI_XValueLinedata.append(self.thread.value[17])
            self.GMI_YValueLinedata.append(self.thread.value[18])
            self.VarValueLinedata.append(self.thread.value[2])

            self.ExtValueLinedata.append(self.thread.value[4])
            VarState = int(self.thread.value[5])
            if (VarState == 2):
                self.VarStateLinedata.append(1)
            elif (VarState == 1):
                self.VarStateLinedata.append(2)
            else:
                self.VarStateLinedata.append(self.thread.value[5])
            ExtState = int(self.thread.value[6])
            if (ExtState == 2):
                self.ExtStateLinedata.append(4)
            elif (ExtState == 1):
                self.ExtStateLinedata.append(5)
            else:
                self.ExtStateLinedata.append(3)
            self.IntensityLinedata.append(self.thread.value[9])
            self.IntensityMiddleLinedata.append(self.thread.value[10])
            IntState = int(self.thread.value[11])
            if (IntState == 2):
                self.IntStateLinedata.append(7)
            elif (IntState == 1):
                self.IntStateLinedata.append(8)
            else:
                self.IntStateLinedata.append(6)
            Result = self.thread.value[12];
            if (Result == 1):
                self.ResultLinedata.append(11)
            else:
                self.ResultLinedata.append(9)


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
        return self.XValueLine, self.YValueLine, self.ZValueLine,self.GMI_XValueLine,self.GMI_YValueLine,self.VarValueLine, self.ExtValueLine, self.VarStateLine, self.ExtStateLine, self.IntensityLine, self.IntensityMiddleLine, self.IntStateLine,self.ResultLine

    def start(self):
        ani = animation.FuncAnimation(self.fig, self.update, init_func=self.init, frames=50, interval=200)

        plt.show()

    def __del__(self):
        print "delete"

    def closeing(self):
        plt.close()


class Scope3D:
    def __init__(self, thread):
        self.thread  = thread
        self.fig = plt.figure()
        self.ax3d = p3.Axes3D(self.fig)
        self.ValueLine, = self.ax3d.plot([], [], "b", lw=2)
        self.MiddleValueLine, = self.ax3d.plot([], [], "r", lw=2)

        self.ax3d.set_xlim3d([0, 4096])
        self.ax3d.set_xlabel('X')

        self.ax3d.set_ylim3d([0, 4096])
        self.ax3d.set_ylabel('Y')

        self.ax3d.set_zlim3d([0, 4096])
        self.ax3d.set_zlabel('Z')

        self.ax3d.set_title('xiaoxiami')

        self.xvalue = []
        self.yvalue = []
        self.zvalue = []
        self.xvalue_middle = []
        self.yvalue_middle = []
        self.zvalue_middle = []
        self.xmax = 2000
        self.ymax = 2000
        self.zmax = 2000
        self.xmin = 1000
        self.ymin = 1000
        self.zmin = 1000




    def update_lines(self,i):
        if self.thread.value[0] * self.thread.value[1] * self.thread.value[16] != 0 and self.thread.value[16] < 4096 and \
                        self.thread.value[0] < 4096 and self.thread.value[1] < 4096:
            self.xmax = max(self.thread.value[0],self.xmax)
            self.ymax = max(self.thread.value[1],self.ymax)
            self.zmax = max(self.thread.value[16],self.zmax)
            self.xmin = min(self.thread.value[0],self.xmin)
            self.ymin = min(self.thread.value[1],self.ymin)
            self.zmin = min(self.thread.value[16],self.zmin)
            self.ax3d.set_xlim3d([self.xmin, self.xmax])
            self.ax3d.set_ylim3d([self.ymin, self.ymax])
            self.ax3d.set_zlim3d([self.zmin, self.zmax])

            self.xvalue.append(self.thread.value[0])
            self.yvalue.append(self.thread.value[1])
            self.zvalue.append(self.thread.value[16])
            self.xvalue_middle.append(self.thread.value[7])
            self.yvalue_middle.append(self.thread.value[8])
            self.zvalue_middle.append(self.thread.value[23])
            self.ValueLine.set_data(self.xvalue,self.yvalue)
            self.ValueLine.set_3d_properties(self.zvalue)
            self.MiddleValueLine.set_data(self.xvalue_middle,self.yvalue_middle)
            self.MiddleValueLine.set_3d_properties(self.zvalue_middle)


        return self.ValueLine,self.MiddleValueLine

    def start(self):

        line_ani = animation.FuncAnimation(self.fig, self.update_lines, 25,
                                   interval=50, blit=False)
        plt.show()
        # import matplotlib
        # matplotlib.use('TkAgg')
        #
        # from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
        #
        # root = Tk.Tk()
        # root.wm_title("Embedding in TK")
        # canvas = FigureCanvasTkAgg(self.fig, master=root)
        # canvas.show()
        # canvas.get_tk_widget().pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)
        # toolbar = NavigationToolbar2TkAgg(canvas, root)
        # toolbar.update()
        # canvas._tkcanvas.pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)
        # def _quit():
        #     root.quit()     # stops mainloop
        #     root.destroy()  # this is necessary on Windows to prevent
        #             # Fatal Python Error: PyEval_RestoreThread: NULL tstate
        #
        # button = Tk.Button(master=root, text='Quit', command=_quit)
        # button.pack(side=Tk.BOTTOM)
        # Tk.mainloop()



    def clearLine(self):
        self.xvalue = []
        self.yvalue = []
        self.zvalue = []

class ScopeOld3D:
    def __init__(self,data,singlefile):
        self.fig = plt.figure()
        self.ax3d = p3.Axes3D(self.fig)
        self.ax3d.set_title('xiaoxiami')
        if singlefile:
            self.ValueLine, = self.ax3d.plot([], [], "b", lw=2)
            self.MiddleValueLine, = self.ax3d.plot([], [], "r", lw=2)
            self.xvalue = []
            self.yvalue = []
            self.zvalue = []

            for item in data[1:]:
                self.xvalue.append(int(item[0]))
                self.yvalue.append(int(item[1]))
                self.zvalue.append(int(item[16]))

            xmax = int(sorted(self.xvalue)[0])
            ymax = int(sorted(self.yvalue)[0])
            zmax = int(sorted(self.zvalue)[0])
            xmin = int(sorted(self.xvalue)[-1])
            ymin = int(sorted(self.yvalue)[-1])
            zmin = int(sorted(self.zvalue)[-1])

            # print xmin,xmax
            # self.ax3d.set_xli3d([886, 1010])
            # self.ax3d.set_xlabel('X')
            #
            # self.ax3d.set_ylim3d([ymin, ymax])
            # self.ax3d.set_ylabel('Y')
            #
            # self.ax3d.set_zlim3d([zmin, zmax])
            # self.ax3d.set_zlabel('Z')
            self.ax3d.plot(xs=self.xvalue,ys=self.yvalue,zs=self.zvalue,label='car')
        else:
            i = 0
            for differentfiles in data:
                XValue = []
                YValue = []
                ZValue = []
                for v in differentfiles[1:]:
                    XValue.append(int(v[0]))
                    YValue.append(int(v[1]))
                    ZValue.append(int(v[16]))
                i += 1
                self.ax3d.plot(xs=XValue,ys=YValue,zs=ZValue,label=i)

        self.ax3d.legend()
        plt.show()

class ScopeOld2D:
    def __init__(self,data):
        x=[]
        y=[]
        firstx=data[0][0][0]
        firsty=data[0][0][1]
        for i in range(len(data)):
            line = data[i]

            for j in range(len(line)):
                dot = line[j]
                if j==0 and i!=0:
                    offsetx = firstx - dot[0]
                    offsety = firsty - dot[1]
                if i!=0:
                    x.append(dot[0] + offsetx)
                    y.append(dot[1] + offsety)
                else:
                    x.append(dot[0])
                    y.append(dot[1])
            plt.plot(x, y,label=1)
            x=[]
            y=[]

        plt.show()

if __name__ == '__main__':
    scope = Scope()
    scope.start()
