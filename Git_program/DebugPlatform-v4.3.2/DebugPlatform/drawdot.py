__author__ = 'chang'
import matplotlib.pyplot as plt
import sys
import matplotlib.animation as animation


class DrawDot():
    def __init__(self, period, method):
        self.period = period
        filename = ["tangentCoordinate.txt"]
        self.filename = filename[int(method)]
        self.dynamicline_data = []
        self.dynamicline = []
        self.xlim_min = 0
        self.ylim_min = 0
        self.xlim_max = 0
        self.ylim_max = 0
        self.frame_count = 0

    def update_line(self, i):
        line_count = 0
        for line in self.dynamicline:
            if self.frame_count < len(self.dynamicline_data[line_count][0]):
                xlist = self.dynamicline_data[line_count][0][0:self.frame_count]
                ylist = self.dynamicline_data[line_count][1][0:self.frame_count]
                line.set_data(xlist, ylist)
                x = float(self.dynamicline_data[line_count][0][self.frame_count])
                y = float(self.dynamicline_data[line_count][1][self.frame_count])

                self.xlim_min = min(self.xlim_min, x)
                self.ylim_min = min(self.ylim_min, y)
                self.xlim_max = max(self.xlim_max, x)
                self.ylim_max = max(self.ylim_max, y)

                self.ax.set_xlim(self.xlim_min - 10, self.xlim_max + 10)
                self.ax.set_ylim(self.ylim_min - 10, self.ylim_max + 10)

            line_count += 1
            # yield line,
        self.frame_count += 1
        return self.dynamicline,

    def init(self):
        for line in self.dynamicline:
            line.set_data([], [])
        return self.dynamicline,

    def getData(self):
        '''
        get data from (funcname).txt
        :return:[(file1)[x,y],(file2)[x,y]]
        '''
        self.dynamicline_data=[]
        with open("../Data/Draw/%s" % self.filename) as file:
            for line in file:
                line = line.split("|")
                x=[]
                y=[]
                for v in line[0].split(" ")[:-1]:
                    x.append(float(v))
                for v in line[1].split(" ")[:-1]:
                    y.append(float(v))
                self.dynamicline_data.append([x,y])

    def drawAnimate(self):
        self.getData()
        fig, self.ax = plt.subplots()
        for i in range(len(self.dynamicline_data)):
            line, = self.ax.plot([], [], 'o')
            self.dynamicline.append(line, )
        self.frames = 0
        for item in self.dynamicline_data:
            for length in item:
                self.frames = max(len(length), self.frames)
        plt.xlim(-10, 100)
        plt.ylim(-10, 10)

        plt.xlabel('xiaoxiami')
        plt.title('Animate')
        line_ani = animation.FuncAnimation(fig, self.update_line, init_func=self.init, frames=self.frames,
                                           interval=int(self.period))
        plt.show()

    def drawStatic(self):
        self.getData()
        fig, ax = plt.subplots()
        for x,y in self.dynamicline_data:
            ax.plot(x, y, "o")
            ax.set_xlim(sorted(x)[0]-10,float(sorted(x)[-1])+10)
            ax.set_ylim(sorted(y)[0]-10,float(sorted(y)[-1])+10)
        plt.xlabel('xiaoxiami')
        plt.title('Static')
        plt.show()



if __name__ == '__main__':
    draw = DrawDot(sys.argv[1], sys.argv[2])
    if sys.argv[3] == "animate":
        draw.drawAnimate()
    elif sys.argv[3] == "static":
        draw.drawStatic()


