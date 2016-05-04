# coding=utf-8
__author__ = 'xiaoxiami'
import Tkinter as tk
import ttk
import ctypes
import platform
import os
import tkMessageBox as tkmes
import tkFont
import time
import linecache
from collections import deque

import tkFileDialog

if platform.system() != "Linux":
    import matplot
    import matplotoldanimate

import uart
import carstop


class MainRoot(tk.Tk):
    """Container for all frames within the application"""

    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)

        # initialize menu
        self.rootmenu = MenuBar(self)
        self.config(menu=self.rootmenu)
        self.title("小虾米 v4.3.1")

        # 居中显示
        curWidth = self.winfo_screenwidth()  # get current width
        curHeight = self.winfo_screenheight()  # get current height
        self.resolution = curWidth * curHeight

        # self.resolution = 53600

        if self.resolution <= 153600:
            tmpcnf = '%dx%d+%d+%d' % (480, 230, 0, 0)
        else:
            scnWidth = (curWidth - 1330) / 2
            scnHeight = (curHeight - 660) / 2  # get screen width and height
            # now generate configuration information
            tmpcnf = '%dx%d+%d+%d' % (1330, 660, scnWidth, scnHeight)

        self.geometry(tmpcnf)
        self.appcanvas = tk.Canvas(self)
        self.appFrame = Application(root=self)
        self.appFrame.pack(side='top', fill='both', expand='True')

        if platform.system() == "Windows":
            if not os.path.exists("../Data/"):
                os.mkdir("../Data/")
            if not os.path.exists("../Data/Identify/"):
                os.mkdir("../Data/Identify/")
            if not os.path.exists("../Data/Filter/"):
                os.mkdir("../Data/Filter/")
            if not os.path.exists("../Data/Temperature/"):
                os.mkdir("../Data/Temperature/")

    def ShowMain(self):
        pass
        # self.rootmenu = MenuBar(self)
        # self.config(menu=self.rootmenu)

        # self.appFrame = Application(root=self)
        # self.appFrame.pack(side='top', fill='both', expand='True')

    def ShowData(self):
        self.appFrame.ShowData()

    def NetStatus(self):
        self.appFrame.NetStatus()

    def StopStatus(self):
        self.appFrame.StopStatus()

    def Command(self):
        self.appFrame.SendCommand()

    def QOSTest(self):
        self.appFrame.QOS()

    def NetSniffer(self):
        if self.resolution > 153600:
            self.appFrame.DrawSensor()

    def SysStatus(self):
        self.status = StatusBar(self)
        self.status.pack(side='bottom', fill='x')

    def Identify(self):
        if self.resolution > 153600:
            self.appFrame.Identify()

    def algorithm(self):
        if platform.system() != "Linux":
            self.appFrame.algorithm()

    def tempTest(self):
        self.appFrame.initTempLabel()


class MenuBar(tk.Menu):
    portlistsort = []

    def __init__(self, parent):
        tk.Menu.__init__(self, parent)
        self.carlocatecboxbuf = '方兴大厦'
        self.updatamodecboxbuf = '开启'
        self.datamodecboxbuf = "串口数据"
        self.root = parent

    def show(self):
        '''
        Parameter：
            none
        Function：
                                绘制菜单选项
        Autor:xiaoxiami 2015.5.29
        Others：网络设置暂时没有使用
        '''
        self.carstoproot = carstop.CarStopRoot(rootframe=self.root)
        filemenu = tk.Menu(self, tearoff=False)

        self.add_cascade(label="选项", underline=0, menu=filemenu)
        filemenu.add_command(label="串口设置...", command=self.Uartettings)
        if platform.system() != "Linux":
            dataformatmenu = tk.Menu(self, tearoff=False)
            data_process_menu = tk.Menu(self, tearoff=False)

            filemenu.add_cascade(label='数据处理...', menu=dataformatmenu)
            dataformatmenu.add_cascade(label='数据导出...', menu=data_process_menu)
            data_process_menu.add_command(label="导出温度数据...", command=self.tempdataToExcel)
            data_process_menu.add_command(label="导出坐标数据...", command=self.identifydataToExcel)
            dataformatmenu.add_command(label="数据过滤...", command=self.identifydataFilter)

        filemenu.add_command(label="停车设置...", command=self.carstopsettings)
        filemenu.add_command(label="退出", underline=1, command=self.quit)
        helpmenu = tk.Menu(self, tearoff=False)
        self.add_cascade(label="帮助", menu=helpmenu)
        helpmenu.add_command(label="完善中")
        self.bordratecboxbuf = '115200'
        self.datasourcecboxbuf = '中继'
        self.datasourcecboxvalue = 5
        self.opened_uart = []

    def Uartettings(self):
        '''
        Parameter：
            none
        Function：
                                串口设置界面，实例化uart.py
        Autor:xiaoxiami 2015.5.29
        Others：显示串口号只适用于windows
        '''
        if platform.system() == "Windows":
            self.portlistsort = []

            uartportlist = uart.GetSerialPorts()
            uartportlist.enumerate_serial_ports()
            for portnum in range(len(uartportlist.portList)):
                self.portlistsort.append(str(uartportlist.portList[portnum][1]))

            import re

            re_digits = re.compile("(\d+)")

            def emb_numbers(s):
                pieces = re_digits.split(s)
                pieces[1::2] = map(int, pieces[1::2])
                return pieces

            def sort_strings_with_emb_numbers(alist):
                aux = [(emb_numbers(s), s) for s in alist]
                aux.sort()
                return [s for __, s in aux]

            self.portlistsort = sort_strings_with_emb_numbers(self.portlistsort)
        elif platform.system() == "Linux":
            self.portlistsort = "/dev/ttyAMA0"
        global root

        self.uartform = uart.UartRoot(comnum=self.portlistsort, frame=root)

        self.root.appFrame.bindtab()
        self.uartform.showupdate()
        self.uartform.protocol("WM_DELETE_WINDOW", self.uart_shutdown_ttk_repeat)  # 防止退出报错
        '''删掉了mainloop'''
        self.uartform.mainloop()

    def tempdataToExcel(self):
        '''
        Parameter：
            none
        Function：
                                将温度测试数据导出为excel
        Autor:xiaoxiami 2015.12.11
        Others：
        '''
        import xlwt
        dirpath = tkFileDialog.askdirectory(initialdir='..\Data\Temperature', title='请选择温度数据所在文件夹')
        if "Temperature" in dirpath:
            dir = os.listdir(dirpath)
            datapath = []
            row = [0 for i in range(12)]
            if len(dir) > 0 and ".txt" not in dir[0]:
                excelname = "..\Data\Temperature\Temperature.xls"
                for d in dir:
                    current_dirpath = os.path.join(dirpath, d)
                    if os.path.isdir(current_dirpath):
                        datapath.append(current_dirpath)
            else:
                excelname = dirpath + "\Temperature.xls"
                datapath.append(dirpath)

            excel = xlwt.Workbook(encoding='ascii')
            worksheet = excel.add_sheet('Temperature')

            style = xlwt.XFStyle()
            style.num_format_str = "Y-m-d H:M:S"

            for i in range(12):
                worksheet.write(row[i], i * 6, label="Node_" + str(i + 1))
                worksheet.write(row[i], i * 6 + 1, label="X" + str(i + 1))
                worksheet.write(row[i], i * 6 + 2, label="Y" + str(i + 1))
                worksheet.write(row[i], i * 6 + 3, label="Z" + str(i + 1))
                worksheet.write(row[i], i * 6 + 4, label="Temp")
            for d in datapath:
                tmp = d.split("\\")[-1]
                temp = tmp.split("_")[-1]

                for i in range(12):
                    filename = "/Node_" + str(i + 1) + ".txt"
                    with open(d + filename) as file:
                        for line in file:
                            nodedata = line.split(",")
                            if len(nodedata) == 4 and int(nodedata[1]) * int(nodedata[2]) * int(nodedata[3]) != 0:
                                row[i] += 1
                                worksheet.write(row[i], i * 6, label=nodedata[0], style=style)
                                worksheet.col(i * 6).width = 0x0d00 + 2000
                                worksheet.write(row[i], i * 6 + 1, label=int(nodedata[1]))
                                worksheet.write(row[i], i * 6 + 2, label=int(nodedata[2]))
                                worksheet.write(row[i], i * 6 + 3, label=int(nodedata[3]))
                                worksheet.write(row[i], i * 6 + 4, label=int(temp))

            excel.save(excelname)
            os.startfile(excelname)
        else:
            tkmes.showerror("路径选择错误！", "没有找到温度数据！")

    def identifydataToExcel(self):
        import xlwt
        datapath = tkFileDialog.askopenfilenames(initialdir='../Data/Identify', filetypes=[("text file", "*.txt")])
        if datapath:
            excel = xlwt.Workbook(encoding='ascii')
            worksheet = excel.add_sheet('data')
            excelname = "..\Data\Identify\Identify.xls"
            style = xlwt.XFStyle()
            style.num_format_str = "Y-m-d H:M:S"
            worksheet.write(0, 0, label="Time")
            worksheet.write(0, 1, label="X Label")
            worksheet.write(0, 2, label="Y Label")
            worksheet.write(0, 3, label="Z Label")
            row_count = 1
            data = []
            for path in datapath:
                with  open(path) as data_file_object:
                    try:
                        all_the_text = data_file_object.read()
                    finally:
                        data_file_object.close()
                    datastr = all_the_text.split("|")
                    databuf = []
                    datatime = []
                    for i in range(len(datastr)):
                        if (i % 2 == 0):
                            if datastr[i] != "" and datastr[i][0] != "I":
                                databuf.append(datastr[i].split(" "))
                        else:
                            datatime.append(datastr[i])
                    for i in range(len(databuf)):
                        if i in range(len(datatime)):
                            data.append([datatime[i], databuf[i][0], databuf[i][1], databuf[i][16]])
                            # timenow = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(float(datatime[i])))
                            # worksheet.write(row_count, 0, label=timenow)
                            # worksheet.col(0).width = 0x0d00 + 2000
                            # worksheet.write(row_count, 1, label=databuf[i][0])
                            # worksheet.write(row_count, 2, label=databuf[i][1])
                            # worksheet.write(row_count, 3, label=databuf[i][16])
                            # row_count += 1

            def f(x):
                return x[0]

            data.sort(key=f)
            for item in data:
                timenow = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(float(item[0])))
                worksheet.write(row_count, 0, label=timenow)
                worksheet.col(0).width = 0x0d00 + 2000
                worksheet.write(row_count, 1, label=int(item[1]))
                worksheet.write(row_count, 2, label=int(item[2]))
                worksheet.write(row_count, 3, label=int(item[3]))
                row_count += 1

            try:
                excel.save(excelname)
                os.startfile(excelname)
            except IOError, err:
                tkmes.showerror("无法打开！", "Excel文件正在被使用")
                print err
        else:
            tkmes.showerror("无法打开！", "没有选择文件！")

    def identifydataFilter(self):

        filenames = tkFileDialog.askopenfilenames(initialdir='../Data/Identify/', filetypes=[("text file", "*.txt")])
        if not filenames:
            tkmes.showerror("无法打开！", "没有找到数据！")
            return
        for file in filenames:
            memory = []
            try:
                dirpath = ""
                filepath = str(file).replace("Identify", "Filter")
                for v in filepath.split("/")[:-1]:
                    dirpath += v + "/"
                if not os.path.exists(dirpath):
                    os.makedirs(dirpath)

                with open(filepath, "w") as write_file:
                    with open(file, "r") as read_file:
                        for line in read_file:
                            linesplit = line.split(" ")[1:-1]
                            if memory:
                                collect_flag = 1
                                for i in range(len(memory)):
                                    if abs(int(memory[i])-int(linesplit[i]))>1000:
                                        collect_flag = 0
                                if collect_flag:
                                    write_file.write(line)
                            memory = linesplit
            except UnicodeEncodeError:
                tkmes.showerror("错误", "文件名路径中不能有汉字!")
                return
        tkmes.showinfo("完成", "处理完成!")

    def carstopsettings(self):
        '''
        Parameter：
            none
        Function：
             停车设置选项界面
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.carstoproot.show()
        self.carstoproot.showupdata()
        self.carnum = self.carstoproot.carnum
        self.carstoproot.protocol("WM_DELETE_WINDOW", self.carstop_shutdown_ttk_repeat)  # 防止退出报错
        '''删掉了mainloop'''
        self.carstoproot.mainloop()

    # 防止退出报错

    def uart_shutdown_ttk_repeat(self):
        '''
        Parameter：
            none
        Function：
                                关闭串口设置是避免报错
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.uartform.eval('::ttk::CancelRepeat')
        self.uartform.destroy()

    def carstop_shutdown_ttk_repeat(self):
        '''
        Parameter：
            none
        Function：
                                关闭停车设置时避免报错
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.carstoproot.eval('::ttk::CancelRepeat')
        self.carstoproot.destroy()


class Application(ttk.Notebook):
    def __init__(self, root):
        ttk.Notebook.__init__(self, root)

        self.tab1 = ttk.Frame(self)
        self.tab2 = ttk.Frame(self)
        self.tab3 = ttk.Frame(self)
        self.tab4 = ttk.Frame(self)
        self.tab5 = ttk.Frame(self)
        self.tab6 = ttk.Frame(self)
        self.tab7 = ttk.Frame(self)
        self.tab8 = ttk.Frame(self)

        self.add(self.tab1, text="停车状态")
        self.add(self.tab2, text="网络拓扑")
        self.add(self.tab3, text="网络抓包")
        self.add(self.tab4, text="绘制图像")
        self.add(self.tab5, text="控制命令")
        self.add(self.tab6, text="链路测试")
        self.add(self.tab7, text="温度测试")
        self.add(self.tab8, text="算法测试")

        self.topline = []
        self.bottomline = []
        self.carnum = 0

        self.carmove = []
        self.stoptext = []
        self.loacname = []

        self.root = root
        self.menu = root.rootmenu

        if self.root.resolution <= 153600:
            self.front = tkFont.Font(size=10, family="黑体")
            self.width = 470
            self.height = 190
        else:
            self.width = 1320
            self.height = 600
            self.front = tkFont.Font(size=50, family="黑体")

        self.admiddleflag = 0

        self.xlim = 0
        self.animateopen = 0

        # self.bind("<<NotebookTabChanged>>", self.updatetab)

        self.rooterimage = tk.PhotoImage(file="image//rooter.gif")
        self.sensorimage = tk.PhotoImage(file="image//sensor.gif")
        self.canvasline = []  # 绘图曲线
        self.cavasverticalline = []  # 绿色竖线
        self.canvasidentifyline = []  # 识别出竖线
        self.canvasoval = []  # 识别画出的圆点
        self.identifyuartopen = 0
        self.globalxym = 0
        self.globalyym = 0
        self.zoomenable = 0
        self.admiddleline = []  # 中值曲线
        self.datacanvaswidth = 0  # canvas宽度 最后一个x坐标值
        self.carnumbind = []
        self.stopedcarnum = 0
        self.tab = 0
        self.algorithm_start = False
        self.matplotopen = False
        self.datapath = ""
        self.matplotoldopen = False
        self.x1 = 0
        self.temp_node1_middle_flag = 0
        self.temp_node2_middle_flag = 0
        self.temp_node3_middle_flag = 0
        self.temp_node4_middle_flag = 0

    def bindtab(self):
        self.bind("<<NotebookTabChanged>>", self.updatetab)

    def updatetab(self, event):
        '''
        Parameter：
            event：事件绑定传入的参数
        Function：
                                将当前的tab值传递给不同线程
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.tab = self.index('current')
        if self.tab == 5:
            try:
                self.QOSUpdateLabel()
            except AttributeError:
                print "Uart didn't open as Linktest.(mainform.py line 301)"
        elif self.tab == 6:
            try:
                self.updateTempLabel()
            except AttributeError:
                print "Uart didn't open as Tempearturetest.(mainform.py line 306)"

    def StopStatus(self):
        '''
        Parameter：
            none
        Function：
                               绘制停车界面，程序运行时显示的两条实线，一条虚线
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.canvas = tk.Canvas(self.tab1, width=self.width, height=self.height)
        self.canvas.grid(sticky=tk.W + tk.E)
        self.canvas.create_line(0, self.height / 2, self.width, self.height / 2, fill='black', dash=(100, 80), width=5)
        self.canvas.create_line(0, self.height / 3, self.width, self.height / 3, fill='black', width=5)
        self.canvas.create_line(0, self.height / 3 * 2, self.width, self.height / 3 * 2, fill='black', width=5)

    def DrawParking(self):
        '''
        Parameter：

        Function：
                               根据停车设置，绘制停车位
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.statusbar = self.root.status
        global root
        self.carnum = root.rootmenu.carstoproot.carnum

        if len(self.topline) > 0:
            for line in self.topline:
                self.canvas.delete(line)
        if len(self.bottomline) > 0:
            for line in self.bottomline:
                self.canvas.delete(line)
        if len(self.stoptext) > 0:
            for text in self.stoptext:
                self.canvas.delete(text)
        if len(self.loacname) > 0:
            for text in self.loacname:
                self.canvas.delete(text)

        self.loacname.append(
            self.canvas.create_text(self.width / 2, self.height / 2, text=root.rootmenu.carlocatecboxbuf,
                                    font=tkFont.Font(size=50, family="黑体"), fill='purple'))

        if self.carnum % 2 == 0:
            topnum = self.carnum / 2
            bottomnum = self.carnum / 2
        else:
            topnum = (self.carnum + 1) / 2
            bottomnum = (self.carnum - 1) / 2
        everytopx = self.width / topnum + 1
        everybottomx = self.width / bottomnum + 1
        for i in range(1, topnum):
            self.topline.append(
                self.canvas.create_line(everytopx * i, 0, everytopx * i, self.height / 3, fill='black', width=5))

        for i in range(1, bottomnum):
            self.bottomline.append(
                self.canvas.create_line(everybottomx * i, self.height / 3 * 2, everybottomx * i, self.height,
                                        fill='black', width=5))
        if self.carnum > 20:
            self.front.configure(size=10)
        elif self.carnum > 14:
            self.front.configure(size=20)
        elif self.carnum > 8:
            self.front.configure(size=32)

        if self.carnum % 2 == 0:
            topwidth = self.width / (self.carnum / 2)
            bottomwidth = topwidth
            topnum = self.carnum / 2
        # bottomnum = topnum
        else:
            topwidth = self.width / ((self.carnum + 1) / 2)
            bottomwidth = self.width / ((self.carnum - 1) / 2)
            topnum = (self.carnum + 1) / 2

    def stopcaronce(self, move):
        if self.root.resolution <= 153600:
            self.front = tkFont.Font(size=13, family="黑体")
        count = 0
        if self.stopedcarnum > self.carnum:
            self.statusbar.setstatus('车辆数：%s，车位数：%s，无法继续停车', str(self.stopedcarnum), str(self.carnum))
            return
        if self.carnum % 2 == 0:
            topwidth = self.width / (self.carnum / 2)
            bottomwidth = topwidth
            topnum = self.carnum / 2
        # bottomnum = topnum
        else:
            topwidth = self.width / ((self.carnum + 1) / 2)
            bottomwidth = self.width / ((self.carnum - 1) / 2)
            topnum = (self.carnum + 1) / 2

        count = 0
        buf = move.split(",")
        self.carmove = []
        for i in buf:
            self.carmove.append(i.split("|"))
        for num, act in self.carmove:
            # 如果该节点编号已经显示过，则只更新显示
            if num in self.carnumbind:
                if act == "1":
                    self.canvas.itemconfigure(('text' + str(num)), text=num + ":\n已停车", fill='red', font=self.front)
                elif act == '0':
                    self.canvas.itemconfigure(('text' + str(num)), text=num + ":\n未停车", fill='green', font=self.front)
                elif act == '2':
                    self.canvas.itemconfigure(('text' + str(num)), text=num + ":\n识别中", fill='blue', font=self.front)
                elif act == '253':
                    self.canvas.itemconfigure(('text' + str(num)), text=num + ":\n掉线", fill='yellow', font=self.front)
                elif act == '252':
                    self.canvas.itemconfigure(('text' + str(num)), text=num + ":\n电量低", fill='yellow', font=self.front)
            else:
                self.carnumbind.append(num)
                if self.stopedcarnum < topnum:
                    if act == '1':
                        self.canvas.create_text(self.stopedcarnum * topwidth + topwidth / 2, self.height / 6,
                                                text=num + ":\n已停车", font=self.front, fill='red',
                                                tag=('text' + str(num)))
                    elif act == '0':
                        self.canvas.create_text(self.stopedcarnum * topwidth + topwidth / 2, self.height / 6,
                                                text=num + ":\n未停车", font=self.front, fill='green',
                                                tag=('text' + str(num)))
                    elif act == '2':
                        self.canvas.create_text(self.stopedcarnum * topwidth + topwidth / 2, self.height / 6,
                                                text=num + ":\n识别中", font=self.front, fill='blue',
                                                tag=('text' + str(num)))
                    elif act == '253':
                        self.canvas.create_text(self.stopedcarnum * topwidth + topwidth / 2, self.height / 6,
                                                text=num + ":\n掉线", font=self.front, fill='yellow',
                                                tag=('text' + str(num)))
                    elif act == '252':
                        self.canvas.create_text(self.stopedcarnum * topwidth + topwidth / 2, self.height / 6,
                                                text=num + ":\n电量低", font=self.front, fill='blue',
                                                tag=('text' + str(num)))
                    elif act == '251':
                        self.canvas.create_text(self.stopedcarnum * topwidth + topwidth / 2, self.height / 6,
                                                text=num + ":\n温度异常", font=self.front, fill='blue',
                                                tag=('text' + str(num)))
                else:
                    if act == '1':
                        self.canvas.create_text((self.stopedcarnum - topnum) * bottomwidth + bottomwidth / 2,
                                                self.height - self.height / 6, text=num + ":\n已停车", font=self.front,
                                                fill='red', tag=('text' + str(num)))
                    elif act == '0':
                        self.canvas.create_text((self.stopedcarnum - topnum) * bottomwidth + bottomwidth / 2,
                                                self.height - self.height / 6, text=num + ":\n未停车", font=self.front,
                                                fill='green', tag=('text' + str(num)))
                    elif act == '2':
                        self.canvas.create_text((self.stopedcarnum - topnum) * bottomwidth + bottomwidth / 2,
                                                self.height - self.height / 6, text=num + ":\n识别中", font=self.front,
                                                fill='blue', tag=('text' + str(num)))
                    elif act == '253':
                        self.canvas.create_text(self.stopedcarnum * topwidth + topwidth / 2, self.height / 6,
                                                text=num + ":\n掉线", font=self.front, fill='yellow',
                                                tag=('text' + str(num)))
                    elif act == '252':
                        self.canvas.create_text(self.stopedcarnum * topwidth + topwidth / 2, self.height / 6,
                                                text=num + ":\n电量低", font=self.front, fill='blue',
                                                tag=('text' + str(num)))
                    elif act == '251':
                        self.canvas.create_text(self.stopedcarnum * topwidth + topwidth / 2, self.height / 6,
                                                text=num + ":\n温度异常", font=self.front, fill='blue',
                                                tag=('text' + str(num)))
                self.stopedcarnum = self.stopedcarnum + 1

    def NetStatus(self):
        '''
        Parameter：

        Function：
                               网络拓扑
        Autor:xiaoxiami 2015.5.29
        Others：暂时没有用
        '''
        pass
        # canv = tk.Canvas(self.tab2, width=1320, height=600, bg="white")
        # canv.grid()
        #
        # canv.create_image(1320 / 2, 50, image=self.rooterimage)
        # canv.create_image(100, 400, image=self.sensorimage)
        # canv.create_line(1320 / 2, 50, 100, 400, fill='green', dash=(100, 80))
        # canv.create_text(100, 480, text="01")
        # canv.create_image(400, 400, image=self.sensorimage)
        # canv.create_line(1320 / 2, 50, 400, 400, fill='green', dash=(100, 80))
        # canv.create_text(400, 480, text="02")
        # canv.create_image(800, 400, image=self.sensorimage)
        # canv.create_line(1320 / 2, 50, 800, 400, fill='green', dash=(100, 80))
        # canv.create_text(800, 480, text="03")
        # canv.create_image(1200, 400, image=self.sensorimage)
        # canv.create_line(1320 / 2, 50, 1200, 400, fill='red', dash=(100, 80))
        # canv.create_text(1200, 480, text="04")

    def ShowData(self):
        '''
        Parameter：

        Function：
                                绘制抓包显示界面
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.tab3.rowconfigure(0, weight=60)
        self.tab3.rowconfigure(1, weight=1)
        self.tab3.columnconfigure(0, weight=1)
        self.tab3.columnconfigure(1, weight=1)
        self.tab3.columnconfigure(2, weight=1)
        self.tab3.columnconfigure(3, weight=1)
        self.tab3.columnconfigure(4, weight=1)
        #         接收frame
        receivegroup = tk.LabelFrame(self.tab3, text="接收区")
        receivegroup.grid(row=0, column=0, columnspan=4, sticky=tk.N + tk.S + tk.E + tk.W)
        #         继续键
        runbutton = ttk.Button(self.tab3, text='继续', command=self.restart)
        runbutton.grid(row=1, column=0, sticky=tk.E)
        #         停止键
        stopbutton = ttk.Button(self.tab3, text='暂停', command=self.pause)
        stopbutton.grid(row=1, column=1, sticky=tk.W)
        #         打开键
        openbutton = ttk.Button(self.tab3, text='打开txt数据', command=self.opentxt)
        openbutton.grid(row=1, column=2, sticky=tk.E)
        #         清空键
        clearbutton = ttk.Button(self.tab3, text='清空', command=self.cleartext)
        clearbutton.grid(row=1, column=2, sticky=tk.W)
        #         接收显示区
        receivegroup.rowconfigure(0, weight=1)
        receivegroup.columnconfigure(0, weight=1)
        #         receivegroup.columnconfigure(1, weight=1)
        self.receivetext = tk.Text(receivegroup)
        receivesb = tk.Scrollbar(self.receivetext)
        receivesb.pack(side=tk.RIGHT, fill=tk.Y)
        receivesb.config(command=self.receivetext.yview)
        self.receivetext.grid(row=0, column=0, sticky=tk.N + tk.S + tk.E + tk.W)
        self.receivetext.config(yscrollcommand=receivesb.set)
        #         receivelb = tk.Listbox(receivegroup)
        #         receivelb.grid(row=0, column=0,sticky=tk.N+tk.S+tk.E+tk.W)
        #         发送Frame
        sendgroup = tk.LabelFrame(self.tab3, text="发送区")
        sendgroup.grid(row=0, column=4, columnspan=2, sticky=tk.N + tk.S + tk.E + tk.W)
        #         发送键
        sendbutton = ttk.Button(self.tab3, text='发送', command=self.SendData)
        sendbutton.grid(row=1, column=4, sticky=tk.E)
        #         发送输入框
        sendgroup.rowconfigure(0, weight=1)
        sendgroup.columnconfigure(0, weight=1)
        inputbuf = tk.Entry(self.tab3, width=15)
        inputbuf.grid(row=1, column=4, sticky=tk.W)
        #         发送显示区
        inputtext = tk.Text(sendgroup)
        inputtext.grid(row=0, column=0, sticky=tk.N + tk.S + tk.E + tk.W)
        inputsb = tk.Scrollbar(inputtext)
        inputsb.pack(side=tk.RIGHT, fill=tk.Y)
        inputsb.config(command=inputtext.yview)
        inputtext.config(yscrollcommand=inputsb.set)
        #         速率选择
        speedgroup = tk.LabelFrame(self.tab3, text="接收速率选择")
        speedgroup.grid(row=1, column=3, columnspan=1, sticky=tk.W)
        speedgroup.rowconfigure(0)
        speedgroup.rowconfigure(1)

        self.radiovalue = tk.IntVar()
        self.radiovalue.set(0)
        tk.Radiobutton(speedgroup, variable=self.radiovalue, text='低速：接收区显示数据', value=0).grid()
        tk.Radiobutton(speedgroup, variable=self.radiovalue, text='高速：数据存储在txt中', value=1).grid()

    def DrawSensor(self):
        pass

    def SendData(self):
        '''
        Parameter：

        Function：
                                将输入框中的内容用串口发送出去
        Autor:xiaoxiami 2015.5.29
        Others：暂时没有用
        '''
        print self.radiovalue.get()

    def pause(self):
        '''
        Parameter：

        Function：
                                暂停抓吧显示
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        snifferthread = self.menu.uartform.snifferthread
        try:
            snifferthread.thread_stop = True
        except NameError:
            tkmes.showwarning("错误", "串口未启动!")
        time.sleep(1)
        self.menu.snifferthread.uart.close()

    def restart(self):
        '''
        Parameter：

        Function：
                               继续抓包显示
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        snifferthread = self.menu.uartform.snifferthread
        try:
            self.menu.snifferthread.openuart()
            snifferthread.thread_stop = False
            snifferthread.data = []
            self.menu.uartform.snifferthread.openfile()
        # uartthread.openfile()
        except NameError:
            tkmes.showwarning("错误", "串口未启动!")

    def updatetext(self):
        '''
        Parameter：
            chars：需要显示的数据
            type：暂时没用
        Function：
                               将当前的数据插入到接收显示框中
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.receivetext.after(500, self.updatetext)
        if (self.tab == 2):
            self.receivetext.insert(1.0, self.menu.uartform.snifferthread.dataintextbuf)
            self.menu.uartform.snifferthread.dataintextbuf = ""
            # try:
            #     self.receivetext.insert(1.0, chars)
            # except:
            #     print "6"

    def cleartext(self):
        '''
        Parameter：

        Function：
                               将接收显示框中的内容全部删除
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.receivetext.delete(1.0, tk.END)

    def opentxt(self):
        '''
        Parameter：

        Function：
                              打开保存的txt数据
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.pause()
        self.menu.uartform.snifferthread.closefile()
        print self.menu.uartform.txtfilname
        os.startfile(self.menu.uartform.txtfilname)

    def algorithmStart(self):
        '''
        Parameter：

        Function：
                              切换开始运行按钮状态
        Autor:xiaoxiami 2015.12.27
        Others：
        '''
        if self.algorithm_start:
            self.algorithm_start_button.configure(background="red", text="开始运行")
            self.root.status.setstatus("算法运行停止")
            self.root.status.setdata("")
            self.algorithm_start = False
        else:
            self.algorithm_start_button.configure(background="green", text="停止运行")
            self.algorithm_start = True
        return self.algorithm_start

    def identifydataToAxis(self, dir, name):
        axis_values = []
        values = []
        filealldatabuf = []
        fileaxisdatabuf = []
        for filename in name:
            filepath = dir + filename
            with open(filepath) as file:
                for lines in file:
                    buf = lines.split("|")[-1].split(" ")[:-1]
                    try:
                        filealldatabuf.append(buf)
                        fileaxisdatabuf.append([int(buf[0]), int(buf[1]), int(buf[16])])
                    except ValueError, err:
                        print err
            axis_values.append(fileaxisdatabuf)
            values.append(filealldatabuf)
            fileaxisdatabuf = []
            filealldatabuf = []
        return axis_values, values

    def algorithm(self):
        '''
        Parameter：

        Function：
                               绘制算法测试界面
        Autor:xiaoxiami 2015.12.26
        Others：
        '''
        self.algorithm_dirpath = ""
        self.filenames = ""

        def _opendata():
            global datapath
            datapath = tkFileDialog.askopenfilenames(initialdir='../Data/Identify', filetypes=[("text file", "*.txt")])
            if datapath:
                for filename in datapath:
                    buf = str(filename).split("/")[-1]
                    inputtext.insert(tk.END, buf.encode("UTF-8") + ";\n")
                self.algorithm_dirpath = ""
                for i in str(datapath[0]).split("/")[:-1]:
                    self.algorithm_dirpath += i + "/"

        def _start():
            settings = {}
            if not self.algorithm_dirpath:
                tkmes.showerror("无法开始", "没有指定数据文件！")
                return
            # if self.algorithmStart():
            import algorithm
            self.filenames = str(inputtext.get(0.0, tk.END).decode("GB2312")).replace("\n", "").split(";")[:-1]
            settings["dir"] = self.algorithm_dirpath
            settings["file"] = self.filenames
            settings["method"] = algorithm_method_combobox.current()
            settings["period"] = periodspinbox.get()
            settings["data"] = self.identifydataToAxis(self.algorithm_dirpath, self.filenames)[0]
            settings["display"] = dot_check.get()
            settings["source"] = [sourcex.get(), sourcey.get(), sourcez.get()]
            self.algorithm_object = algorithm.Algotithm(settings, self)
            # self.root.status.setstatus(
            #     "开始运行%s算法",
            #     algorithm_method_combobox.get()
            # )
            self.updateAlgorithmlabel()

        def _draw():
            self.filenames = str(inputtext.get(0.0, tk.END).decode("GB2312")).replace("\n", "").split(";")[:-1]
            if not self.filenames:
                tkmes.showinfo("无法绘图", "没有选择数据文件")
                return
            if x_check.get() == '1' and y_check.get() == '1' and z_check.get() == '1':
                data = self.identifydataToAxis(self.algorithm_dirpath, self.filenames)[1]
                matplot.ScopeOld3D(data=data, singlefile=False)
            elif x_check.get() == '0' and y_check.get() == '0' and z_check.get() == '0':
                tkmes.showinfo("无法绘图", "没有选择要绘制的坐标轴")
                return
            else:
                buf = self.identifydataToAxis(self.algorithm_dirpath, self.filenames)[0]
                a = []
                data = []
                # 将两个轴的坐标抽取出来
                if x_check.get() == "1" and y_check.get() == "1":
                    for i in buf:
                        for j in i:
                            a.append([j[0], j[1]])
                        data.append(a)
                        a = []
                elif y_check.get() == "1" and z_check.get() == "1":
                    for i in buf:
                        for j in i:
                            a.append([j[1], j[2]])
                        data.append(a)
                        a = []
                elif z_check.get() == "1" and x_check.get() == "1":
                    for i in buf:
                        for j in i:
                            a.append([j[0], j[2]])
                        data.append(a)
                        a = []
                else:
                    tkmes.showinfo("无法绘图", "不能只选择一个坐标轴")
                    return

                matplot.ScopeOld2D(data)

        for i in range(12):
            self.tab8.columnconfigure(i, weight=1)


        # 运行配置
        settings_frame = tk.LabelFrame(self.tab8, text="运行配置")
        settings_frame.grid(row=0, column=10, columnspan=5, sticky=tk.S + tk.N + tk.E + tk.W)
        settings_frame.columnconfigure(0, weight=1)
        settings_frame.columnconfigure(1, weight=1)

        self.periodString = tk.StringVar()
        tk.Label(settings_frame, text="采集周期(ms)：").grid(row=0, column=0, sticky=tk.E)
        periodspinbox = tk.Spinbox(settings_frame, from_=50, to=1000, increment=100, textvariable=self.periodString)
        self.periodString.set("50")
        periodspinbox.grid(row=0, column=1, sticky=tk.W)
        tk.Label(settings_frame, text="运行算法：").grid(row=1, column=0, sticky=tk.E)
        algorithm_method_combobox = ttk.Combobox(settings_frame)
        algorithm_method_combobox.grid(row=1, column=1, sticky=tk.W)
        # algorithm_method.bind("<<ComboboxSelected>>", self.IsOpen)
        algorithm_method_combobox['value'] = ["切线坐标", "直径", "周长", "紧致度", "平面角", "模拟图像截取"]
        algorithm_method_combobox.set("紧致度")

        datapathbutton = ttk.Button(settings_frame, text="选择数据文件", command=_opendata)
        datapathbutton.grid(row=3, column=0, rowspan=4, sticky=tk.E)
        inputtext = tk.Text(settings_frame)
        inputtext.grid(row=3, column=1, rowspan=4, sticky=tk.N + tk.S + tk.E + tk.W)
        inputsb = tk.Scrollbar(inputtext)
        inputsb.pack(side=tk.RIGHT, fill=tk.Y)
        inputsb.config(command=inputtext.yview)
        inputtext.config(yscrollcommand=inputsb.set)

        tk.Label(settings_frame, text="结果显示方式:").grid(row=7, column=0, sticky=tk.E)
        dot_check = tk.StringVar()
        dot_checkbutton = tk.Checkbutton(
            settings_frame,
            text="动态散点图",
            variable=dot_check,
            onvalue="dot_animate"
        )
        dot_checkbutton.grid(row=7, column=1, sticky=tk.W)
        dot_checkbutton.select()

        tk.Checkbutton(
            settings_frame,
            text="静态散点图",
            variable=dot_check,
            onvalue="dot_static"
        ).grid(row=8, column=1, sticky=tk.W)

        tk.Label(settings_frame, text="数据源:").grid(row=9, column=0, sticky=tk.E)
        sourcex = tk.StringVar()
        sourcex_check = tk.Checkbutton(
            settings_frame,
            text="X",
            variable=sourcex,
        )
        sourcex_check.deselect()
        sourcex_check.grid(row=9, column=1, sticky=tk.W)
        sourcey = tk.StringVar()
        sourcey_check = tk.Checkbutton(
            settings_frame,
            text="Y",
            variable=sourcey,
        )
        sourcey_check.deselect()
        sourcey_check.grid(row=10, column=1, sticky=tk.W)
        sourcez = tk.StringVar()
        sourcez_check = tk.Checkbutton(
            settings_frame,
            text="Z",
            variable=sourcez,
        )
        sourcez_check.deselect()
        sourcez_check.grid(row=11, column=1, sticky=tk.W)

        self.algorithm_start_button = ttk.Button(settings_frame, text="运行", command=_start)
        self.algorithm_start_button.grid(row=12, columnspan=2)

        # 分析工具
        analysis_frame = tk.LabelFrame(self.tab8, text="分析工具")
        analysis_frame.grid(row=1, column=10, columnspan=2, sticky=tk.S + tk.N + tk.E + tk.W)

        # for i in range(4):
        #     analysis_frame.columnconfigure(i, weight=1)

        x_check = tk.StringVar()
        xcheckbutton = tk.Checkbutton(analysis_frame, text="X", variable=x_check)
        xcheckbutton.grid(row=3, column=0)
        xcheckbutton.deselect()
        y_check = tk.StringVar()
        ycheckbutton = tk.Checkbutton(analysis_frame, text="Y", variable=y_check)
        ycheckbutton.grid(row=3, column=1)
        ycheckbutton.deselect()
        z_check = tk.StringVar()
        zcheckbutton = tk.Checkbutton(analysis_frame, text="Z", variable=z_check)
        zcheckbutton.grid(row=3, column=2, sticky=tk.E)
        zcheckbutton.deselect()
        draw3dbutton = ttk.Button(analysis_frame, text="绘制图像", command=_draw)
        draw3dbutton.grid(row=3, column=3)

        # Lavel init

        variable_show_frame = tk.LabelFrame(self.tab8, text="变量显示")
        variable_show_frame.grid(row=0, column=0, rowspan=2, columnspan=10, sticky=tk.S + tk.N + tk.E + tk.W)
        for i in range(10):
            variable_show_frame.columnconfigure(i, weight=1)
        rowandcloumn = 0
        self.v00Label = tk.StringVar()
        self.afterLabel = tk.Label(variable_show_frame, textvariable=self.v00Label).grid(row=rowandcloumn / 10,
                                                                                         column=rowandcloumn % 10)
        rowandcloumn += 1
        self.v00String = tk.StringVar()
        tk.Label(variable_show_frame,
                 text="0", textvariable=self.v00String,
                 ).grid(
            row=rowandcloumn / 2,
            column=rowandcloumn % 2,
            columnspan=9
        )
        rowandcloumn += 1
        self.v00String.set("")
        self.v00Label.set("Diameter:")

        self.v10Label = tk.StringVar()
        self.afterLabel = tk.Label(
            variable_show_frame,
            textvariable=self.v10Label
        ).grid(
            row=rowandcloumn / 2,
            column=rowandcloumn % 2
        )
        rowandcloumn += 1
        self.v10String = tk.StringVar()
        tk.Label(variable_show_frame,
                 text="0", textvariable=self.v10String,
                 ).grid(
            row=rowandcloumn / 2,
            column=rowandcloumn % 2,
            columnspan=9
        )
        rowandcloumn += 1
        self.v10String.set("")
        self.v10Label.set("Perimeter:")

        self.v20Label = tk.StringVar()

        self.afterLabel = tk.Label(
            variable_show_frame,
            textvariable=self.v20Label
        )
        self.afterLabel.grid(
            row=rowandcloumn / 2,
            column=rowandcloumn % 2
        )
        rowandcloumn += 1
        self.v20String = tk.StringVar()
        tk.Label(variable_show_frame,
                 text="0", textvariable=self.v20String,
                 ).grid(
            row=rowandcloumn / 2,
            column=rowandcloumn % 2,
            columnspan=9
        )
        rowandcloumn += 1
        self.v20String.set("")
        self.v20Label.set("Compatness:")
        # Lavel init end

    def updateAlgorithmlabel(self):
        self.v00String.set(self.algorithm_object.diameter)
        self.v10String.set(self.algorithm_object.perimeter)
        self.v20String.set(self.algorithm_object.compatness)
        self.afterLabel.after(500, self.updateAlgorithmlabel)

    def Identify(self):
        '''
        Parameter：

        Function：
                               绘制识别界面
        Autor:xiaoxiami 2015.8.30
        Others：
        '''

        self.tab4.rowconfigure(0, weight=60)
        self.tab4.rowconfigure(1, weight=1)
        self.tab4.columnconfigure(0, weight=1)
        self.tab4.columnconfigure(1, weight=1)
        self.tab4.columnconfigure(2, weight=1)
        self.tab4.columnconfigure(3, weight=1)
        self.tab4.columnconfigure(4, weight=1)
        self.datapathentry = tk.Entry(self.tab4)
        self.datapathentry.grid(row=1, column=0, sticky=tk.E + tk.W)
        self.datapathbutton = ttk.Button(self.tab4, text="选择文件", command=self.Selectdata)
        self.datapathbutton.grid(row=1, column=1, sticky=tk.W)

        if platform.system() != "Linux":
            self.dataRPbutton = tk.Button(self.tab4, command=self.MatplotlibDrawing, background="red", text="显示2D图像")
            self.dataRPbutton.grid(row=1, column=3)
            self.datashow3dbutton = tk.Button(self.tab4, command=self.drawing3D, background="red", text="显示3D图像")
            self.datashow3dbutton.grid(row=1, column=3, sticky=tk.E)
            self.datashow3dclearbutton = tk.Button(self.tab4, command=self.Clear3DLine, text="清屏")
            self.datashow3dclearbutton.grid(row=1, column=4, sticky=tk.W)
            self.dataclearbutton = tk.Button(self.tab4, text="清屏", command=self.Cleardata)
            self.dataclearbutton.grid(row=1, column=3, padx=20, sticky=tk.W, )

        self.newtextbutton = tk.Button(self.tab4, command=self.newtext, text="新文件")
        self.newtextbutton.grid(row=1, column=4)

        self.datascale = ttk.Scale(self.tab4, orient=tk.HORIZONTAL, from_=1, to=50, command=self.Zoomcallback)
        self.datascale.grid(row=1, column=2, sticky=tk.W)
        self.datascalevalue = 1
        self.datascale.set(self.datascalevalue)
        self.datascalelabel = ttk.Label(self.tab4, text="1")
        self.datascalelabel.grid(row=1, column=2, sticky=tk.E)

        receivegroup = tk.LabelFrame(self.tab4, text="历史数据")
        receivegroup.grid(row=0, column=0, columnspan=3, sticky=tk.N + tk.S + tk.E + tk.W)

        self.datacavas = tk.Canvas(receivegroup)
        # self.datacavas.grid(row=0, column=0, columnspan=2, sticky=tk.N + tk.S + tk.E + tk.W)
        self.cancassbx = tk.Scrollbar(self.datacavas, orient=tk.HORIZONTAL)
        # self.cancassbx.pack(side=tk.BOTTOM, fill=tk.X)
        self.cancassbx.config(command=self.datacavas.xview)
        self.datacavas.config(xscrollcommand=self.cancassbx.set)

        # 历史数据控制界面绘制
        self.endtimeentry_string = tk.StringVar()
        self.starttimeentry_string = tk.StringVar()
        history_file_path = []
        self.history_queue = deque()
        self.history_last_queue = deque()
        self.live_data = []
        self.history_dirpath = ""
        self.history_start = 0
        self.history_frame_count = 0
        self.history_update_interval = 50

        class Data():
            self.value = []

            def set(self, data):
                self.value = data

        thread = Data()

        def _open_history_data():
            history_file_path = []
            datapath = tkFileDialog.askopenfilenames(initialdir='../Data/Filter', filetypes=[("text file", "*.txt")])
            timestr = []
            if datapath:
                self.history_dirpath = ""
                for i in str(datapath[0]).split("/")[:-1]:
                    self.history_dirpath += i + "/"
                for filename in datapath:
                    buf = str(filename).split("/")[-1]
                    historyinputtext.insert(tk.END, buf.encode("UTF-8") + ";\n")
                    history_file_path.append(self.history_dirpath + buf.encode("GB2312"))
                    timestr.append(linecache.getlines(self.history_dirpath + buf.encode("GB2312"))[0].split("|")[1])
                    timestr.append(linecache.getlines(self.history_dirpath + buf.encode("GB2312"))[-1].split("|")[1])
            if not timestr:
                return
            timestr.sort()
            self.endtimeentry_string.set(time.strftime('%H:%M:%S  %Y-%m-%d', time.localtime(float(timestr[-1]))))
            self.starttimeentry_string.set((time.strftime('%H:%M:%S  %Y-%m-%d', time.localtime(float(timestr[0])))))

        def _process_data():
            self.filenames = str(historyinputtext.get(0.0, tk.END).decode("GB2312")).replace("\n", "").split(";")[:-1]
            if not self.filenames:
                tkmes.showerror("错误！", "没有选择数据文件！")
                return
            try:
                starttime = time.mktime(time.strptime(self.starttimeentry_string.get(), '%H:%M:%S  %Y-%m-%d'))
                endtime = time.mktime(time.strptime(self.endtimeentry_string.get(), '%H:%M:%S  %Y-%m-%d'))
            except:
                tkmes.showerror("错误！", "时间格式错误！")
                return
            if starttime > endtime:
                tkmes.showerror("错误！", "结束时间不能在开始时间之前！")
                return
            self.history_queue.clear()
            self.filenames = str(historyinputtext.get(0.0, tk.END).decode("GB2312")).replace("\n", "").split(";")[:-1]
            for filename in self.filenames:
                with open(self.history_dirpath + filename) as file:
                    for line in file:
                        current_time = float(line.split("|")[1])
                        if current_time > starttime and current_time < endtime:
                            self.history_queue.append(line)

            tkmes.showinfo("完成", "数据读取完成!")
            self.history_framecount.set(0)
            self.history_frame_count = 0

        def _continue_run():
            if not self.history_queue:
                tkmes.showinfo("提示", "没有读取到数据")
                return
            self.history_frame_count = int(self.history_framecount.get().split("/")[0])
            self.history_start = 1
            self.history_update_interval = int(self.history_periodString.get())
            _periodupdatelabel()
            if self.history_radiovalue.get() == '1':
                matplot.Scope3D(thread).start()

        def _stop():
            self.history_start = 0

        def _updatelabel(frame_data):
            self.XValueString.set(frame_data[0])
            self.YValueString.set(frame_data[1])
            self.VarianceString.set(frame_data[2])
            self.Ext_MiddleString.set(frame_data[3])
            self.ExtremumString.set(frame_data[4])
            self.VarStateString.set(frame_data[5])
            self.ExtStateString.set(frame_data[6])
            self.XMiddleString.set(frame_data[7])
            self.YMiddleString.set(frame_data[8])
            self.IntensityeString.set(frame_data[9])
            middle_value = int(frame_data[10])
            self.IntStateString.set(frame_data[11])
            self.ResultString.set(frame_data[12])
            self.update_middle_times_string.set(ctypes.c_int16(int(frame_data[13])).value)
            self.XValue_parked_stable_string.set(ctypes.c_int16(int(frame_data[14])).value)
            self.YValue_parked_stable_string.set(ctypes.c_int16(int(frame_data[15])).value)
            self.ZValueString.set(frame_data[16])
            self.GMI_XvalueString.set(frame_data[17])
            self.GMI_YvalueString.set(frame_data[18])
            self.GMI_XvalueMiddleString.set(frame_data[19])
            self.GMI_YvalueMiddleString.set(frame_data[20])
            self.VoltageString.set(frame_data[21])
            self.ZValue_parked_stable_string.set(frame_data[22])
            self.ZMiddleString.set(frame_data[23])
            switch_middle = int(frame_data[24])
            self.switch_middleString.set(switch_middle)

            if switch_middle == 0:
                self.x_middle_quene0String.set(middle_value)
            elif switch_middle == 1:
                self.x_middle_quene1String.set(middle_value)
            elif switch_middle == 2:
                self.x_middle_quene2String.set(middle_value)
            elif switch_middle == 3:
                self.x_middle_quene3String.set(middle_value)
            elif switch_middle == 4:
                self.x_middle_quene4String.set(middle_value)
            elif switch_middle == 10:
                self.y_middle_quene0String.set(middle_value)
            elif switch_middle == 11:
                self.y_middle_quene1String.set(middle_value)
            elif switch_middle == 12:
                self.y_middle_quene2String.set(middle_value)
            elif switch_middle == 13:
                self.y_middle_quene3String.set(middle_value)
            elif switch_middle == 14:
                self.y_middle_quene4String.set(middle_value)
            elif switch_middle == 20:
                self.z_middle_quene0String.set(middle_value)
            elif switch_middle == 21:
                self.z_middle_quene1String.set(middle_value)
            elif switch_middle == 22:
                self.z_middle_quene2String.set(middle_value)
            elif switch_middle == 23:
                self.z_middle_quene3String.set(middle_value)
            elif switch_middle == 24:
                self.z_middle_quene4String.set(middle_value)
            self.reverse_flagString.set(frame_data[25])
            self.parked_distanceString.set(frame_data[26])
            self.diameterbuf_string.set(frame_data[27])
            self.CompatnessString.set(frame_data[28])
            self.parking_timeString.set(frame_data[29])

        def _last_frame(event=0):
            if self.history_frame_count == 1:
                tkmes.showinfo("提示", "已经到第一帧")
                return
            # frame_data = str(self.history_last_queue.pop())
            if (self.history_frame_count >= len(self.history_queue)):
                self.history_frame_count = len(self.history_queue) - 1
            # self.history_queue.appendleft(frame_data)
            frame_data = self.history_queue[self.history_frame_count]
            frame_data = frame_data.split("|")[2].split(" ")[:-1]
            self.history_frame_count -= 1
            self.history_framecount.set(str(self.history_frame_count) + "/" + str(len(self.history_queue)))
            _updatelabel(frame_data)

        def _next_frame(event=0):
            if self.history_frame_count == len(self.history_queue):
                tkmes.showinfo("提示", "已经到第最后一帧")
                return
            # frame_data = str(self.history_queue.popleft())
            frame_data = self.history_queue[self.history_frame_count]
            # self.history_last_queue.append(frame_data)
            frame_data = frame_data.split("|")[2].split(" ")[:-1]
            self.history_frame_count += 1
            self.history_framecount.set(str(self.history_frame_count) + "/" + str(len(self.history_queue)))
            _updatelabel(frame_data)

        def _periodupdatelabel():
            if self.history_start == 0:
                return
            if not self.history_queue:
                return
            if self.history_frame_count >= len(self.history_queue):
                return
            frame_data = self.history_queue[self.history_frame_count]
            frame_data = frame_data.split("|")[2].split(" ")[:-1]
            self.history_frame_count += 1
            self.history_framecount.set(str(self.history_frame_count) + "/" + str(len(self.history_queue)))
            intdata = []
            for d in frame_data:
                intdata.append(int(d))
            thread.set(intdata)
            _updatelabel(frame_data)

            self.XValueLabel.after(self.history_update_interval, _periodupdatelabel)

        for i in range(10):
            receivegroup.rowconfigure(i, weight=1)
        for i in range(4):
            receivegroup.columnconfigure(i, weight=1)

        historyinputtext = tk.Text(receivegroup)
        historyinputtext.grid(row=0, column=0, columnspan=10)
        rowandcloumn = 4
        tk.Label(receivegroup, text="开始时间:").grid(row=rowandcloumn / 4, column=rowandcloumn % 4)

        rowandcloumn += 1

        tk.Entry(receivegroup, textvariable=self.starttimeentry_string).grid(row=rowandcloumn / 4,
                                                                             column=rowandcloumn % 4)

        rowandcloumn += 1
        ttk.Button(
            receivegroup,
            text="选择数据文件",
            command=_open_history_data
        ).grid(row=rowandcloumn / 4, column=rowandcloumn % 4)

        rowandcloumn += 1
        ttk.Button(
            receivegroup,
            text="读取数据",
            command=_process_data
        ).grid(row=rowandcloumn / 4, column=rowandcloumn % 4)

        rowandcloumn += 1
        tk.Label(receivegroup, text="结束时间:").grid(row=rowandcloumn / 4, column=rowandcloumn % 4)

        rowandcloumn += 1
        tk.Entry(receivegroup, textvariable=self.endtimeentry_string).grid(row=rowandcloumn / 4,
                                                                           column=rowandcloumn % 4)

        rowandcloumn += 1
        ttk.Button(
            receivegroup,
            text="上一帧",
            command=_last_frame
        ).grid(row=rowandcloumn / 4, column=rowandcloumn % 4)

        rowandcloumn += 1
        ttk.Button(
            receivegroup,
            text="下一帧",
            command=_next_frame
        ).grid(row=rowandcloumn / 4, column=rowandcloumn % 4)

        rowandcloumn += 1
        self.history_periodString = tk.StringVar()
        tk.Label(receivegroup, text="周期(ms)：").grid(row=rowandcloumn / 4, column=rowandcloumn % 4)
        periodspinbox = tk.Spinbox(receivegroup, from_=50, to=1000, increment=100,
                                   textvariable=self.history_periodString)
        rowandcloumn += 1
        periodspinbox.grid(row=rowandcloumn / 4, column=rowandcloumn % 4)
        self.history_periodString.set("50")

        rowandcloumn += 1
        ttk.Button(
            receivegroup,
            text="连续运行",
            command=_continue_run
        ).grid(row=rowandcloumn / 4, column=rowandcloumn % 4)

        rowandcloumn += 1
        ttk.Button(
            receivegroup,
            text="停止运行",
            command=_stop
        ).grid(row=rowandcloumn / 4, column=rowandcloumn % 4)

        rowandcloumn += 1
        self.history_radiovalue = tk.StringVar()
        self.history_radiovalue.set(3)
        tk.Radiobutton(
            receivegroup,
            variable=self.history_radiovalue,
            value=1,
            text="3D图像"
        ).grid(row=rowandcloumn / 4, column=rowandcloumn % 4)
        rowandcloumn += 1
        tk.Radiobutton(
            receivegroup,
            variable=self.history_radiovalue,
            value=2,
            text="2D图像"
        ).grid(row=rowandcloumn / 4, column=rowandcloumn % 4)
        rowandcloumn += 1
        tk.Radiobutton(
            receivegroup,
            variable=self.history_radiovalue,
            value=3,
            text="不绘图"
        ).grid(row=rowandcloumn / 4, column=rowandcloumn % 4)

        rowandcloumn += 1
        rowandcloumn += 1
        tk.Label(receivegroup, text="当前帧数：").grid(row=rowandcloumn / 4, column=rowandcloumn % 4)
        rowandcloumn += 1
        self.history_framecount = tk.StringVar()
        tk.Label(
            receivegroup,
            text="",
            textvariable=self.history_framecount
        ).grid(row=rowandcloumn / 4, column=rowandcloumn % 4)
        self.history_framecount.set("0")

        readgroup = tk.LabelFrame(self.tab4, text="变量显示")
        readgroup.grid(row=0, column=3, columnspan=3, sticky=tk.N + tk.S + tk.E + tk.W)

        for i in range(10):
            readgroup.rowconfigure(i, weight=1)
        for i in range(10):
            readgroup.columnconfigure(i, weight=1)

        # init Label
        rowandcloumn = 0

        self.XValueString = tk.StringVar()
        tk.Label(readgroup, text="XValue:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.XValueLabel = tk.Label(readgroup, text="0", textvariable=self.XValueString, width=4)
        self.XValueLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.XValueString.set(0)

        self.XMiddleString = tk.StringVar()
        tk.Label(readgroup, text="XMiddle:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.AD_middle_valueXLabel = tk.Label(readgroup, text="0", textvariable=self.XMiddleString, width=4)
        self.AD_middle_valueXLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.XMiddleString.set(0)

        self.GMI_XvalueString = tk.StringVar()
        tk.Label(readgroup, text="XValue_Stable:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.GMI_XValueLabel = tk.Label(readgroup, text="0", textvariable=self.GMI_XvalueString, width=4)
        self.GMI_XValueLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.GMI_XvalueString.set(0)

        self.YValueString = tk.StringVar()
        tk.Label(readgroup, text="YValue:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.YValueLabel = tk.Label(readgroup, text="0", textvariable=self.YValueString, width=4)
        self.YValueLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.YValueString.set(0)

        self.YMiddleString = tk.StringVar()
        tk.Label(readgroup, text="YMiddle:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.AD_middle_valueYLabel = tk.Label(readgroup, text="0", textvariable=self.YMiddleString, width=4)
        self.AD_middle_valueYLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.YMiddleString.set(0)

        self.GMI_YvalueString = tk.StringVar()
        tk.Label(readgroup, text="YValue_Stable:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.GMI_YValueLabel = tk.Label(readgroup, text="0", textvariable=self.GMI_YvalueString, width=4)
        self.GMI_YValueLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.GMI_YvalueString.set(0)

        self.ZValueString = tk.StringVar()
        tk.Label(readgroup, text="ZValue:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.ZvalueLabel = tk.Label(readgroup, text="0", textvariable=self.ZValueString, width=4)
        self.ZvalueLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.ZValueString.set(0)

        self.ZMiddleString = tk.StringVar()
        tk.Label(readgroup, text="ZMiddle:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.AD_middle_valueZLabel = tk.Label(readgroup, text="0", textvariable=self.ZMiddleString, width=4)
        self.AD_middle_valueZLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.ZMiddleString.set(0)

        self.GMI_XvalueMiddleString = tk.StringVar()
        tk.Label(readgroup, text="ZValue_Stable:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.GMI_XValueMiddleLabel = tk.Label(readgroup, text="0", textvariable=self.GMI_XvalueMiddleString, width=4)
        self.GMI_XValueMiddleLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.GMI_XvalueMiddleString.set(0)

        self.VarianceString = tk.StringVar()
        tk.Label(readgroup, text="Variance:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.VarianceMLabel = tk.Label(readgroup, text="0", textvariable=self.VarianceString, width=4)
        self.VarianceMLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.VarianceString.set(0)

        self.ExtremumString = tk.StringVar()
        tk.Label(readgroup, text="Extremum:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.ExtremumValueLabel = tk.Label(readgroup, text="0", textvariable=self.ExtremumString, width=4)
        self.ExtremumValueLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.ExtremumString.set(0)

        self.Ext_MiddleString = tk.StringVar()
        tk.Label(readgroup, text="Ext_Middle:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.ExtremumValueMiddleLabel = tk.Label(readgroup, text="0", textvariable=self.Ext_MiddleString, width=4)
        self.ExtremumValueMiddleLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.Ext_MiddleString.set(0)

        self.IntensityeString = tk.StringVar()
        tk.Label(readgroup, text="Distance:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.IntensityLabel = tk.Label(readgroup, text="0", textvariable=self.IntensityeString, width=4)
        self.IntensityLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.IntensityeString.set(0)

        self.Int_MiddleString = tk.StringVar()
        tk.Label(readgroup, text="Int_Middle:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.IntensityMiddleLabel = tk.Label(readgroup, text="0", textvariable=self.Int_MiddleString, width=4)
        self.IntensityMiddleLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.Int_MiddleString.set(0)

        self.VarStateString = tk.StringVar()
        tk.Label(readgroup, text="VarState:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.VarStateLabel = tk.Label(readgroup, text="0", textvariable=self.VarStateString, width=4)
        self.VarStateLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.VarStateString.set(0)

        self.ExtStateString = tk.StringVar()
        tk.Label(readgroup, text="ExtState:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.ExtStateLabel = tk.Label(readgroup, text="0", textvariable=self.ExtStateString, width=4)
        self.ExtStateLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.ExtStateString.set(0)

        self.IntStateString = tk.StringVar()
        tk.Label(readgroup, text="IntState:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.IntStateLabel = tk.Label(readgroup, text="0", textvariable=self.IntStateString, width=4)
        self.IntStateLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.IntStateString.set(0)

        self.ResultString = tk.StringVar()
        tk.Label(readgroup, text="Result:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.ResultLabel = tk.Label(readgroup, text="0", textvariable=self.ResultString, width=4)
        self.ResultLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.ResultString.set(0)

        self.ZValue_parked_stable_string = tk.StringVar()
        tk.Label(readgroup, text="ZValue_parked_stable:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.ZValue_parked_stable_string, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.ZValue_parked_stable_string.set(0)

        self.switch_middleString = tk.StringVar()
        tk.Label(readgroup, text="switch_middle:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.switch_middleString, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.switch_middleString.set(0)

        self.x_middle_quene0String = tk.StringVar()
        tk.Label(readgroup, text="x_middle[0]:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.x_middle_quene0String, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.x_middle_quene0String.set(0)

        self.y_middle_quene0String = tk.StringVar()
        tk.Label(readgroup, text="y_middle[0]:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.y_middle_quene0String, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.y_middle_quene0String.set(0)

        self.z_middle_quene0String = tk.StringVar()
        tk.Label(readgroup, text="z_middle[0]:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.z_middle_quene0String, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.z_middle_quene0String.set(0)

        self.XValue_parked_stable_string = tk.StringVar()
        tk.Label(readgroup, text="XValue_parked_stable:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.YAve_SlopLabel = tk.Label(readgroup, text="0", textvariable=self.XValue_parked_stable_string, width=4)
        self.YAve_SlopLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.XValue_parked_stable_string.set(0)

        self.update_middle_times_string = tk.StringVar()
        tk.Label(readgroup, text="update_middle_times:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.XAve_SlopLabel = tk.Label(readgroup, text="0", textvariable=self.update_middle_times_string, width=4)
        self.XAve_SlopLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.update_middle_times_string.set(0)

        self.x_middle_quene1String = tk.StringVar()
        tk.Label(readgroup, text="x_middle[1]:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.x_middle_quene1String, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.x_middle_quene1String.set(0)

        self.y_middle_quene1String = tk.StringVar()
        tk.Label(readgroup, text="y_middle[1]:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.y_middle_quene1String, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.y_middle_quene1String.set(0)

        self.z_middle_quene1String = tk.StringVar()
        tk.Label(readgroup, text="z_middle[1]:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.z_middle_quene1String, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.z_middle_quene1String.set(0)

        self.YValue_parked_stable_string = tk.StringVar()
        tk.Label(readgroup, text="YValue_parked_stable:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.ZAve_SlopLabel = tk.Label(readgroup, text="0", textvariable=self.YValue_parked_stable_string, width=4)
        self.ZAve_SlopLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.YValue_parked_stable_string.set(0)

        self.parking_timeString = tk.StringVar()
        tk.Label(readgroup, text="perimeterbuf:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.parking_timeString, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.parking_timeString.set(0)

        self.x_middle_quene2String = tk.StringVar()
        tk.Label(readgroup, text="x_middle[2]:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.x_middle_quene2String, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.x_middle_quene2String.set(0)

        self.y_middle_quene2String = tk.StringVar()
        tk.Label(readgroup, text="y_middle[2]:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.y_middle_quene2String, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.y_middle_quene2String.set(0)

        self.z_middle_quene2String = tk.StringVar()
        tk.Label(readgroup, text="z_middle[2]:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.z_middle_quene2String, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.z_middle_quene2String.set(0)

        self.VoltageString = tk.StringVar()
        tk.Label(readgroup, text="Z_parked_distance:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.VoltageLabel = tk.Label(readgroup, text="0", textvariable=self.VoltageString, width=4)
        self.VoltageLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.VoltageString.set(0)

        self.reverse_flagString = tk.StringVar()
        tk.Label(readgroup, text="toggle_reason:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.YValue_ParkingLabel = tk.Label(readgroup, text="0", textvariable=self.reverse_flagString, width=4)
        self.YValue_ParkingLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.reverse_flagString.set(0)

        self.x_middle_quene3String = tk.StringVar()
        tk.Label(readgroup, text="x_middle[3]:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.x_middle_quene3String, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.x_middle_quene3String.set(0)

        self.y_middle_quene3String = tk.StringVar()
        tk.Label(readgroup, text="y_middle[3]:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.y_middle_quene3String, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.y_middle_quene3String.set(0)

        self.z_middle_quene3String = tk.StringVar()
        tk.Label(readgroup, text="z_middle[3]:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.z_middle_quene3String, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.z_middle_quene3String.set(0)

        self.parked_distanceString = tk.StringVar()
        tk.Label(readgroup, text="parked_distance:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.InfraredLabel = tk.Label(readgroup, text="0", textvariable=self.parked_distanceString, width=4)
        self.InfraredLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.parked_distanceString.set(0)

        self.diameterbuf_string = tk.StringVar()
        tk.Label(readgroup, text="diameterbuf:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.DistanceLabel = tk.Label(readgroup, text="0", textvariable=self.diameterbuf_string, width=4)
        self.DistanceLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.diameterbuf_string.set(0)

        self.x_middle_quene4String = tk.StringVar()
        tk.Label(readgroup, text="x_middle[4]:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.x_middle_quene4String, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.x_middle_quene4String.set(0)

        self.y_middle_quene4String = tk.StringVar()
        tk.Label(readgroup, text="y_middle[4]:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.y_middle_quene4String, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.y_middle_quene4String.set(0)

        self.z_middle_quene4String = tk.StringVar()
        tk.Label(readgroup, text="z_middle[4]:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        tk.Label(readgroup, text="0", textvariable=self.z_middle_quene4String, width=4) \
            .grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.z_middle_quene4String.set(0)

        self.GMI_YvalueMiddleString = tk.StringVar()
        tk.Label(readgroup, text="ZValue_HP:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.GMI_YValueMiddleLabel = tk.Label(readgroup, text="0", textvariable=self.GMI_YvalueMiddleString, width=4)
        self.GMI_YValueMiddleLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.GMI_YvalueMiddleString.set(0)

        self.CompatnessString = tk.StringVar()
        tk.Label(readgroup, text="Compactness:").grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.CompatnessLabel = tk.Label(readgroup, text="0", textvariable=self.CompatnessString, width=10)
        self.CompatnessLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.CompatnessString.set(0)



        # self.datatext = tk.Text(readgroup)
        # self.datatext.grid(row=0, column=0, sticky=tk.N + tk.S + tk.E + tk.W)

        # inputsb = tk.Scrollbar(self.datatext)
        # inputsb.pack(side=tk.RIGHT, fill=tk.Y)
        # inputsb.config(command=self.datatext.yview)
        # self.datatext.config(yscrollcommand=inputsb.set)

    def Clear3DLine(self):
        try:
            self.scope.clearLine()
        except:
            pass

    def Cleardata(self):
        '''
        Parameter：

        Function：
                              清空识别界面
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        pass

    def confirm(self):
        '''
        Parameter：

        Function：
                              输入新的文件名
        Autor:xiaoxiami 2015.9.05
        Others：
        '''
        self.filenameadd = self.entry.get()
        if not self.filenameadd:
            self.filenameadd = time.strftime('%Y-%m-%d_%H-%M-%S', time.localtime(time.time()))
        # self.filenameinput.quit()
        self.root.status.setstatus('../Data/Identify/' + self.filenameadd + '.txt')
        self.menu.uartform.identifythread.filename = '../Data/Identify/' + self.filenameadd + '.txt'

    def newtext(self):
        '''
        Parameter：

        Function：
                              新建一个TXT文件，添加备注文件名
        Autor:xiaoxiami 2015.9.05
        Others：
        '''

        if (self.identifyuartopen != 0):
            self.filenameinput = tk.Tk()
            self.entry = tk.Entry(self.filenameinput)
            enter = tk.Button(self.filenameinput, text="确定", command=self.confirm)
            self.entry.pack()
            enter.pack()
            # self.filenameinput.mainloop()

    def MatplotlibDrawing(self):
        '''
        Parameter：

        Function：
                              用matplotlib 绘图
        Autor:xiaoxiami 2015.8.30
        Others：
        '''
        if (self.datapath == ""):
            if (self.identifyuartopen == 0):
                tkmes.showerror("错误！", "串口没有打开！\n请手动载入数据或打开串口！")
                return
            self.identifythread = self.menu.uartform.identifythread
            self.updatelabel()
            if self.identifythread.thread_stop == False:
                self.identifythread.thread_stop = True
                self.dataRPbutton.configure(background="red", text="显示2D图像")
            else:
                self.identifythread.thread_stop = False
                self.dataRPbutton.configure(background="green", text="显示2D图像")
                self.scope = matplot.Scope(thread=self.menu.uartform.identifythread)
                self.scope.start()
        else:

            self.matplotanimate = matplotoldanimate.Scope(data=self.filedata, thread=self)
            self.matplotanimate.start()

    def drawing3D(self):
        '''
        Parameter：

        Function：
                              用matplotlib 绘图
        Autor:xiaoxiami 2015.8.30
        Others：
        '''

        if self.datapath == "":
            if self.identifyuartopen == 0:
                tkmes.showerror("错误！", "串口没有打开！\n请手动载入数据或打开串口！")
                return

            if isinstance(self.menu.uartform.identifythread.value[0], int):
                self.identifythread = self.menu.uartform.identifythread
                self.updatelabel()
                if self.identifythread.thread_stop == False:
                    self.identifythread.thread_stop = True
                    self.datashow3dbutton.configure(background="red", text="显示3D图像")
                else:

                    self.identifythread.thread_stop = False
                    self.datashow3dbutton.configure(background="green", text="显示3D图像")
                    self.scope = matplot.Scope3D(thread=self.menu.uartform.identifythread)
                    self.scope.start()


        else:
            print self.filedata
            if isinstance(self.filedata[0][0], str):
                matplot.ScopeOld3D(data=self.filedata, singlefile=True)
            else:
                matplot.ScopeOld3D(data=self.filedata, singlefile=False)

    def Selectdata(self):
        '''
        Parameter：

        Function：
                              选择txt文件路径
        Autor:xiaoxiami 2015.8.30
        Others：默认打开目录为 F:\Graduate\Git_program\DebugPlatform-v3.0\Data
        '''

        self.datapath = tkFileDialog.askopenfilenames(initialdir='../Data/Identify', filetypes=[("text file", "*.txt")])
        self.datapathentry.delete(0, tk.END)
        self.datapathentry.insert(1, self.datapath)
        self.zoomenable = 1
        if len(self.datapath) <= 1:
            try:
                self.data_file_object = open(self.datapath[0])
            except:
                tkmes.showerror("错误！", "未能找到文件，请检查文件路径！")
                return
            try:
                all_the_text = self.data_file_object.read()
            finally:
                self.data_file_object.close()
            datastr = all_the_text.split("|")
            databuf = []
            datatime = []
            for i in range(len(datastr)):
                if (i % 2 == 0):
                    if (datastr[i] != ""):
                        databuf.append(datastr[i].split(" "))
                else:
                    datatime.append(datastr[i])

            data = []
            j = 0
            for i in databuf:
                data.append([])
                for v in i:
                    if v != "":
                        data[j].append(v)
                j += 1
            self.filedata = data
            self.Opendata(self.filedata)
        else:
            self.filedata = []
            for eachfile in self.datapath:
                with open(eachfile) as file:
                    all_the_text = file.read()
                    datastr = all_the_text.split("|")
                    databuf = []
                    datatime = []
                    for i in range(len(datastr)):
                        if (i % 2 == 0):
                            if (datastr[i] != ""):
                                databuf.append(datastr[i].split(" "))
                        else:
                            datatime.append(datastr[i])
                    data = []
                    j = 0
                    for i in databuf:
                        data.append([])
                        for v in i:
                            if v != "":
                                data[j].append(v)
                        j += 1
                self.filedata.append(data)

    def Opendata(self, data):
        '''
        Parameter：

        Function：
                              将选定的txt文件内容读取，分类存储在list中
        Autor:xiaoxiami 2015.8.30
        Others：加变量需要修改
        '''
        self.ExtState = []
        self.VarState = []
        self.XValue = []
        self.YValue = []
        self.ZValue = []
        self.GMI_XValue = []
        self.GMI_YValue = []
        self.VarianceM = []
        self.ExtremumValueMiddle = []
        self.ExtremumValue = []
        self.AD_middle_valueX = []
        self.AD_middle_valueY = []
        self.Intensity = []
        self.IntensityMiddle = []
        self.IntState = []
        self.Result = []
        self.XAve_Slop = []
        self.YAve_Slop = []
        self.GMI_XValueM = []
        self.GMI_YValueM = []

        # 加变量需要修改
        # if(len(data) == 10):
        for v in data[1:]:
            self.XValue.append(v[0])
            self.YValue.append(v[1])
            self.ZValue.append(v[16])
            self.GMI_XValue.append(v[17])
            self.GMI_YValue.append(v[18])
            self.GMI_XValueM.append(v[19])
            self.GMI_YValueM.append(v[20])
            self.VarianceM.append(v[2])
            self.ExtremumValueMiddle.append(v[3])
            self.ExtremumValue.append(v[4])
            self.ExtState.append(v[5])
            self.VarState.append(v[6])
            self.AD_middle_valueX.append(v[7])
            self.AD_middle_valueY.append(v[8])
            self.Intensity.append(v[9])
            self.IntensityMiddle.append(v[10])
            self.IntState.append(v[11])
            self.Result.append(v[12])
            self.XAve_Slop.append(v[13])
            self.YAve_Slop.append(v[14])

    def keyxlim(self, event):

        if (event.keysym == "Right"):
            self.x1 += 10
        elif (event.keysym == "Left"):
            self.x1 -= 10
        self.Showdetaildata(0)

    def Zoomcallback(self, event):
        '''
        Parameter：
            event：bind事件
        Function：
                               根据self.datascalevalue重新绘制图像
        Autor:xiaoxiami 2015.8.30
        Others：
        '''
        self.datascalevalue = int(self.datascale.get())
        try:
            #             防止没有datascalelabel
            self.datascalelabel.config(text=str(self.datascalevalue))
            self.matplotanimate.setScalueTo()
        except:
            pass
        if (self.zoomenable == 0):
            return
        self.Opendata(self.filedata)

    def updatelabel(self):
        '''
        Parameter：

        Function：
                               串口打开时实时更新变量显示
        Autor:xiaoxiami 2015.9.17
        Others：
        '''
        self.XValueString.set(self.menu.uartform.identifythread.value[0])
        self.YValueString.set(self.menu.uartform.identifythread.value[1])
        self.VarianceString.set(self.menu.uartform.identifythread.value[2])
        self.Ext_MiddleString.set(self.menu.uartform.identifythread.value[3])
        self.ExtremumString.set(self.menu.uartform.identifythread.value[4])
        self.VarStateString.set(self.menu.uartform.identifythread.value[5])
        self.ExtStateString.set(self.menu.uartform.identifythread.value[6])
        self.XMiddleString.set(self.menu.uartform.identifythread.value[7])
        self.YMiddleString.set(self.menu.uartform.identifythread.value[8])
        self.IntensityeString.set(self.menu.uartform.identifythread.value[9])
        middle_value = int(self.menu.uartform.identifythread.value[10])
        self.IntStateString.set(self.menu.uartform.identifythread.value[11])
        self.ResultString.set(self.menu.uartform.identifythread.value[12])
        self.update_middle_times_string.set(self.menu.uartform.identifythread.value[13])
        self.XValue_parked_stable_string.set(self.menu.uartform.identifythread.value[14])
        self.YValue_parked_stable_string.set(self.menu.uartform.identifythread.value[15])
        self.ZValueString.set(self.menu.uartform.identifythread.value[16])
        self.GMI_XvalueString.set(self.menu.uartform.identifythread.value[17])
        self.GMI_YvalueString.set(self.menu.uartform.identifythread.value[18])
        self.GMI_XvalueMiddleString.set(self.menu.uartform.identifythread.value[19])
        self.GMI_YvalueMiddleString.set(self.menu.uartform.identifythread.value[20])
        self.VoltageString.set(self.menu.uartform.identifythread.value[21])
        self.ZValue_parked_stable_string.set(self.menu.uartform.identifythread.value[22])
        self.ZMiddleString.set(self.menu.uartform.identifythread.value[23])
        switch_middle = self.menu.uartform.identifythread.value[24]
        self.switch_middleString.set(switch_middle)
        if switch_middle == 0:
            self.x_middle_quene0String.set(middle_value)
        elif switch_middle == 1:
            self.x_middle_quene1String.set(middle_value)
        elif switch_middle == 2:
            self.x_middle_quene2String.set(middle_value)
        elif switch_middle == 3:
            self.x_middle_quene3String.set(middle_value)
        elif switch_middle == 4:
            self.x_middle_quene4String.set(middle_value)
        elif switch_middle == 10:
            self.y_middle_quene0String.set(middle_value)
        elif switch_middle == 11:
            self.y_middle_quene1String.set(middle_value)
        elif switch_middle == 12:
            self.y_middle_quene2String.set(middle_value)
        elif switch_middle == 13:
            self.y_middle_quene3String.set(middle_value)
        elif switch_middle == 14:
            self.y_middle_quene4String.set(middle_value)
        elif switch_middle == 20:
            self.z_middle_quene0String.set(middle_value)
        elif switch_middle == 21:
            self.z_middle_quene1String.set(middle_value)
        elif switch_middle == 22:
            self.z_middle_quene2String.set(middle_value)
        elif switch_middle == 23:
            self.z_middle_quene3String.set(middle_value)
        elif switch_middle == 24:
            self.z_middle_quene4String.set(middle_value)
        self.reverse_flagString.set(self.menu.uartform.identifythread.value[25])
        self.parked_distanceString.set(self.menu.uartform.identifythread.value[26])
        self.diameterbuf_string.set(self.menu.uartform.identifythread.value[27])
        self.CompatnessString.set(self.menu.uartform.identifythread.value[28])
        self.parking_timeString.set(self.menu.uartform.identifythread.value[29])

        self.XValueLabel.after(50, self.updatelabel)

    def Showdetaildata(self, event):
        '''
        Parameter：
            event：bind事件
        Function：
                                点击图像时显示该点的数据值
        Autor:xiaoxiami 2015.9.17
        Others：
        '''
        x1 = 0

        self.statusbar = self.root.status
        self.offset = int(self.datascale.get())

        try:
            # self.DrawOldData.datascalelabel.config(text=str(x1 / self.offset))
            x1 = event.x + int(self.cancassbx.get()[0] * len(self.ExtState) * self.offset)
            self.x1 = x1
            self.matplotanimate.setScaleValue()
        except:
            x1 = event + int(self.cancassbx.get()[0] * len(self.ExtState) * self.offset)
        self.xlim = x1 / self.offset
        if (x1 >= 0 and x1 < len(self.XValue) * self.offset):
            self.statusbar.setdata("%s", "Count:" + str(x1 / self.offset))
            # self.statusbar.setdata("%s","方差:" + str(self.VarState[x1 / self.offset]) + ",极值:" + str(self.ExtState[x1 / self.offset]))
            self.XValueString.set(self.XValue[x1 / self.offset])
            self.YValueString.set(self.YValue[x1 / self.offset])
            self.ZValueString.set(self.ZValue[x1 / self.offset])
            self.GMI_XvalueString.set(self.GMI_XValue[x1 / self.offset])
            self.GMI_YvalueString.set(self.GMI_YValue[x1 / self.offset])
            self.GMI_XvalueMiddleString.set(self.GMI_XValueM[x1 / self.offset])
            self.GMI_YvalueMiddleString.set(self.GMI_YValueM[x1 / self.offset])
            self.VarianceString.set(self.VarianceM[x1 / self.offset])

            self.XMiddleString.set(self.AD_middle_valueX[x1 / self.offset])
            self.YMiddleString.set(self.AD_middle_valueY[x1 / self.offset])
            self.ExtremumString.set(self.ExtremumValue[x1 / self.offset])
            self.Ext_MiddleString.set(self.ExtremumValueMiddle[x1 / self.offset])
            self.IntensityeString.set(self.Intensity[x1 / self.offset])
            self.Int_MiddleString.set(self.IntensityMiddle[x1 / self.offset])
            self.IntStateString.set(self.IntState[x1 / self.offset])
            self.ResultString.set(self.Result[x1 / self.offset])
            self.IntensityMinusString.set(
                abs(int(self.Intensity[x1 / self.offset]) - int(self.IntensityMiddle[x1 / self.offset])))
            self.update_middle_times_string.set(ctypes.c_int16(int(self.XAve_Slop[x1 / self.offset])))
            self.XValue_parked_stable_string.set(ctypes.c_int16(int(self.YAve_Slop[x1 / self.offset])))



            # if(self.matplotoldopen == True):
            #     if(self.animateopen == 0):
            #         self.animateopen = 1
            #         self.matplotanimate = matplotoldanimate.Scope(data = self.filedata,thread = self)
            #         self.matplotanimate.start()
            # self.DrawOldData.set_xlim(x1/self.offset)

    def QOS(self):
        '''
        Parameter：

        Function：
                                链路质量测试
        Autor:xiaoxiami 2015.10.17
        Others：
        '''
        for i in range(10):
            self.tab6.rowconfigure(i, weight=1)
        for i in range(10):
            self.tab6.columnconfigure(i, weight=1)
        if self.root.resolution <= 153600:
            txtfont = tkFont.Font(size=10, family="黑体")
        else:
            txtfont = tkFont.Font(size=20, family="黑体")
        rowandcloumn = 0

        self.PacketSend_CountString = tk.StringVar()
        tk.Label(self.tab6, text="节点发送包数量:", font=txtfont).grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.PacketSend_CountLabel = tk.Label(self.tab6, text="0", textvariable=self.PacketSend_CountString, width=5,
                                              font=txtfont)
        self.PacketSend_CountLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.PacketSend_CountString.set(0)

        self.PacketReceive_CountString = tk.StringVar()
        tk.Label(self.tab6, text="中继接收包数量:", font=txtfont).grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.PacketReceive_CountLabel = tk.Label(self.tab6, text="0", textvariable=self.PacketReceive_CountString,
                                                 width=5, font=txtfont)
        self.PacketReceive_CountLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.PacketReceive_CountString.set(0)

        self.AckLost_CountString = tk.StringVar()
        tk.Label(self.tab6, text="ACK丢失数量:", font=txtfont).grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.AckLost_CountLabel = tk.Label(self.tab6, text="0", textvariable=self.AckLost_CountString, width=5,
                                           font=txtfont)
        self.AckLost_CountLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.AckLost_CountString.set(0)

        self.Lost_PersentString = tk.StringVar()
        tk.Label(self.tab6, text="丢包率:", font=txtfont).grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.Lost_PersentLabel = tk.Label(self.tab6, text="0", textvariable=self.Lost_PersentString, width=6,
                                          font=txtfont)
        self.Lost_PersentLabel.grid(row=rowandcloumn / 10, column=rowandcloumn % 10)
        rowandcloumn += 1
        self.Lost_PersentString.set(0)

    def QOSUpdateLabel(self):
        '''
        Parameter：

        Function：
                                更新链路质量测试显示
        Autor:xiaoxiami 2015.10.17
        Others：
        '''

        self.PacketSend_CountString.set(self.menu.uartform.linktestthread.value[0])
        self.AckLost_CountString.set(self.menu.uartform.linktestthread.value[1])
        self.Lost_PersentString.set(self.menu.uartform.linktestthread.value[2])
        self.PacketReceive_CountString.set(self.menu.uartform.linktestthread.value[3])
        self.PacketSend_CountLabel.after(50, self.QOSUpdateLabel)

    def SendCommand(self):
        '''
        Parameter：

        Function：
                                向节点发送命令
        Autor:xiaoxiami 2015.10.15
        Others：
        '''

        def cmdtext(cmd):
            address = cmdinput.get()
            try:
                self.menu.uartform.relaythread.SendCmd(address, cmd)
            except:
                tkmes.showerror("错误！", "串口未打开，无法发送！")

        for i in range(6):
            self.tab5.columnconfigure(i, weight=1)
        phyaddtxtfont = tkFont.Font(size=20, family="黑体")
        tk.Label(self.tab5, text="物理地址：", font=phyaddtxtfont).grid(row=0, column=1, sticky=tk.E)
        cmdinput = tk.Entry(self.tab5, width=15)
        cmdinput.grid(row=0, column=2, sticky=tk.W)
        buttonfont = tkFont.Font(size=16, family="黑体")
        cmdbutton1 = tk.Button(self.tab5, text="测试数据", command=lambda: cmdtext(1), font=buttonfont)
        cmdbutton2 = tk.Button(self.tab5, text="未停车", command=lambda: cmdtext(2), font=buttonfont)
        cmdbutton3 = tk.Button(self.tab5, text="已停车", command=lambda: cmdtext(3), font=buttonfont)
        cmdbutton4 = tk.Button(self.tab5, text="重启", command=lambda: cmdtext(4), font=buttonfont)
        cmdbutton5 = tk.Button(self.tab5, text="数据回传", command=lambda: cmdtext(5), font=buttonfont)
        cmdbutton1.grid(row=1, column=0)
        cmdbutton2.grid(row=1, column=1)
        cmdbutton3.grid(row=1, column=2)
        cmdbutton4.grid(row=1, column=3)
        cmdbutton5.grid(row=1, column=4)

    def initTempLabel(self):
        '''
        Parameter：

        Function：
                               显示温漂测试标题
        Autor:xiaoxiami 2015.11.29
        Others：
        '''

        def buttonCallback(event=0):
            successflag = 1
            if temperatureinput.get():
                tempearture = temperatureinput.get()
                try:
                    if int(tempearture) > 90 or int(tempearture) < -50:
                        successflag = 0
                        self.temp_starttime_String.set("测试温度输入有误")
                except:
                    successflag = 0
                    self.temp_starttime_String.set("测试温度输入有误")
                if successflag:
                    self.temp_starttime_String.set(
                        "测试开始：" + time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time())))
                    try:
                        self.menu.uartform.temperaturethread.filepath, self.menu.uartform.temperaturethread.file = \
                            self.menu.uartform.temperaturethread.createFileandPath(tempearture)
                        self.menu.uartform.temperaturethread.started = True
                    except AttributeError, err:
                        successflag = 0
                        self.temp_starttime_String.set("请配置串口")
                        print err
            else:
                self.temp_starttime_String.set("请输入测试温度")

        for i in range(9):
            self.tab7.rowconfigure(i, weight=1)
        for i in range(14):
            self.tab7.columnconfigure(i, weight=1)

        if self.root.resolution <= 153600:
            txtfont = tkFont.Font(size=10, family="黑体")
        else:
            txtfont = tkFont.Font(size=20, family="黑体")

        tk.Label(self.tab7, text="测试温度:", font=txtfont).grid(row=8, column=0, columnspan=6)
        temperatureinput = tk.Entry(self.tab7, width=10)
        temperatureinput.grid(row=8, column=3, sticky=tk.W, columnspan=3)
        tk.Label(self.tab7, text="℃", font=txtfont).grid(row=8, column=3, columnspan=2)
        cmdbutton1 = tk.Button(self.tab7, text="开始测试", font=txtfont, command=buttonCallback)
        cmdbutton1.grid(row=8, column=0, columnspan=12)
        self.temp_starttime_String = tk.StringVar()
        tk.Label(self.tab7, textvariable=self.temp_starttime_String, width=30, font=txtfont).grid(row=8, column=4,
                                                                                                  columnspan=13)
        self.temp_starttime_String.set("")
        temperatureinput.bind("<KeyPress-Return>", buttonCallback)

        self.node1_num_String = tk.StringVar()
        tk.Label(self.tab7, text="节点编号:", font=txtfont).grid(row=0, column=0, sticky=tk.W)
        self.node1_num_Label = tk.Label(self.tab7, text="0", textvariable=self.node1_num_String, width=8, font=txtfont)
        self.node1_num_Label.grid(row=0, column=1, sticky=tk.W)
        self.node1_num_String.set(0)

        self.node1_Xmiddle_String = tk.StringVar()
        self.node1_Ymiddle_String = tk.StringVar()
        self.node1_Zmiddle_String = tk.StringVar()
        tk.Label(self.tab7, text="标定值:X:", font=txtfont).grid(row=1, column=0, sticky=tk.W)
        tk.Label(self.tab7, text="Y:", font=txtfont).grid(row=1, column=2)
        tk.Label(self.tab7, text="Z:", font=txtfont).grid(row=1, column=4)
        self.node1_Xmiddle_Label = tk.Label(self.tab7, text="0", textvariable=self.node1_Xmiddle_String, width=4,
                                            font=txtfont)
        self.node1_Ymiddle_Label = tk.Label(self.tab7, text="0", textvariable=self.node1_Ymiddle_String, width=4,
                                            font=txtfont)
        self.node1_Zmiddle_Label = tk.Label(self.tab7, text="0", textvariable=self.node1_Zmiddle_String, width=4,
                                            font=txtfont)
        self.node1_Xmiddle_Label.grid(row=1, column=1)
        self.node1_Ymiddle_Label.grid(row=1, column=3)
        self.node1_Zmiddle_Label.grid(row=1, column=5)
        self.node1_Xmiddle_String.set(0)
        self.node1_Ymiddle_String.set(0)
        self.node1_Zmiddle_String.set(0)

        self.node1_Xvalue_String = tk.StringVar()
        self.node1_Yvalue_String = tk.StringVar()
        self.node1_Zvalue_String = tk.StringVar()
        tk.Label(self.tab7, text="当前值:X:", font=txtfont).grid(row=2, column=0, sticky=tk.W)
        tk.Label(self.tab7, text="Y:", font=txtfont).grid(row=2, column=2)
        tk.Label(self.tab7, text="Z:", font=txtfont).grid(row=2, column=4)
        self.node1_Xvalue_Label = tk.Label(self.tab7, text="0", textvariable=self.node1_Xvalue_String, width=4,
                                           font=txtfont)
        self.node1_Yvalue_Label = tk.Label(self.tab7, text="0", textvariable=self.node1_Yvalue_String, width=4,
                                           font=txtfont)
        self.node1_Zvalue_Label = tk.Label(self.tab7, text="0", textvariable=self.node1_Zvalue_String, width=4,
                                           font=txtfont)
        self.node1_Xvalue_Label.grid(row=2, column=1)
        self.node1_Yvalue_Label.grid(row=2, column=3)
        self.node1_Zvalue_Label.grid(row=2, column=5)
        self.node1_Xvalue_String.set(0)
        self.node1_Yvalue_String.set(0)
        self.node1_Zvalue_String.set(0)

        self.node1_time_String = tk.StringVar()
        tk.Label(self.tab7, text="测试时间:", font=txtfont).grid(row=3, column=0, sticky=tk.W)
        self.node1_time_Label = tk.Label(self.tab7, text="0", textvariable=self.node1_time_String, width=26,
                                         font=txtfont)
        self.node1_time_Label.grid(row=3, column=1, sticky=tk.W, columnspan=3)
        self.node1_time_String.set(0)

        self.node2_num_String = tk.StringVar()
        tk.Label(self.tab7, text="节点编号:", font=txtfont).grid(row=0, column=7, sticky=tk.W)
        self.node2_num_Label = tk.Label(self.tab7, text="0", textvariable=self.node2_num_String, width=8, font=txtfont)
        self.node2_num_Label.grid(row=0, column=8, sticky=tk.W)
        self.node2_num_String.set(0)

        self.node2_Xmiddle_String = tk.StringVar()
        self.node2_Ymiddle_String = tk.StringVar()
        self.node2_Zmiddle_String = tk.StringVar()
        tk.Label(self.tab7, text="标定值:X:", font=txtfont).grid(row=1, column=7, sticky=tk.W)
        tk.Label(self.tab7, text="Y:", font=txtfont).grid(row=1, column=9)
        tk.Label(self.tab7, text="Z:", font=txtfont).grid(row=1, column=11)
        self.node2_Xmiddle_Label = tk.Label(self.tab7, text="0", textvariable=self.node2_Xmiddle_String, width=4,
                                            font=txtfont)
        self.node2_Ymiddle_Label = tk.Label(self.tab7, text="0", textvariable=self.node2_Ymiddle_String, width=4,
                                            font=txtfont)
        self.node2_Zmiddle_Label = tk.Label(self.tab7, text="0", textvariable=self.node2_Zmiddle_String, width=4,
                                            font=txtfont)
        self.node2_Xmiddle_Label.grid(row=1, column=8)
        self.node2_Ymiddle_Label.grid(row=1, column=10)
        self.node2_Zmiddle_Label.grid(row=1, column=12)
        self.node2_Xmiddle_String.set(0)
        self.node2_Ymiddle_String.set(0)
        self.node2_Zmiddle_String.set(0)

        self.node2_Xvalue_String = tk.StringVar()
        self.node2_Yvalue_String = tk.StringVar()
        self.node2_Zvalue_String = tk.StringVar()
        tk.Label(self.tab7, text="当前值:X:", font=txtfont).grid(row=2, column=7, sticky=tk.W)
        tk.Label(self.tab7, text="Y:", font=txtfont).grid(row=2, column=9)
        tk.Label(self.tab7, text="Z:", font=txtfont).grid(row=2, column=11)
        self.node2_Xvalue_Label = tk.Label(self.tab7, text="0", textvariable=self.node2_Xvalue_String, width=4,
                                           font=txtfont)
        self.node2_Yvalue_Label = tk.Label(self.tab7, text="0", textvariable=self.node2_Yvalue_String, width=4,
                                           font=txtfont)
        self.node2_Zvalue_Label = tk.Label(self.tab7, text="0", textvariable=self.node2_Zvalue_String, width=4,
                                           font=txtfont)
        self.node2_Xvalue_Label.grid(row=2, column=8)
        self.node2_Yvalue_Label.grid(row=2, column=10)
        self.node2_Zvalue_Label.grid(row=2, column=12)
        self.node2_Xvalue_String.set(0)
        self.node2_Yvalue_String.set(0)
        self.node2_Zvalue_String.set(0)

        self.node2_time_String = tk.StringVar()
        tk.Label(self.tab7, text="测试时间:", font=txtfont).grid(row=3, column=7, sticky=tk.W)
        self.node2_time_Label = tk.Label(self.tab7, text="0", textvariable=self.node2_time_String, width=26,
                                         font=txtfont)
        self.node2_time_Label.grid(row=3, column=8, sticky=tk.W, columnspan=3)
        self.node2_time_String.set(0)

        self.node3_num_String = tk.StringVar()
        tk.Label(self.tab7, text="节点编号:", font=txtfont).grid(row=4, column=0, sticky=tk.W)
        self.node3_num_Label = tk.Label(self.tab7, text="0", textvariable=self.node3_num_String, width=8, font=txtfont)
        self.node3_num_Label.grid(row=4, column=1, sticky=tk.W)
        self.node3_num_String.set(0)

        self.node3_Xmiddle_String = tk.StringVar()
        self.node3_Ymiddle_String = tk.StringVar()
        self.node3_Zmiddle_String = tk.StringVar()
        tk.Label(self.tab7, text="标定值:X:", font=txtfont).grid(row=5, column=0, sticky=tk.W)
        tk.Label(self.tab7, text="Y:", font=txtfont).grid(row=5, column=2)
        tk.Label(self.tab7, text="Z:", font=txtfont).grid(row=5, column=4)
        self.node3_Xmiddle_Label = tk.Label(self.tab7, text="0", textvariable=self.node3_Xmiddle_String, width=4,
                                            font=txtfont)
        self.node3_Ymiddle_Label = tk.Label(self.tab7, text="0", textvariable=self.node3_Ymiddle_String, width=4,
                                            font=txtfont)
        self.node3_Zmiddle_Label = tk.Label(self.tab7, text="0", textvariable=self.node3_Zmiddle_String, width=4,
                                            font=txtfont)
        self.node3_Xmiddle_Label.grid(row=5, column=1)
        self.node3_Ymiddle_Label.grid(row=5, column=3)
        self.node3_Zmiddle_Label.grid(row=5, column=5)
        self.node3_Xmiddle_String.set(0)
        self.node3_Ymiddle_String.set(0)
        self.node3_Zmiddle_String.set(0)

        self.node3_Xvalue_String = tk.StringVar()
        self.node3_Yvalue_String = tk.StringVar()
        self.node3_Zvalue_String = tk.StringVar()
        tk.Label(self.tab7, text="当前值:X:", font=txtfont).grid(row=6, column=0, sticky=tk.W)
        tk.Label(self.tab7, text="Y:", font=txtfont).grid(row=6, column=2)
        tk.Label(self.tab7, text="Z:", font=txtfont).grid(row=6, column=4)
        self.node3_Xvalue_Label = tk.Label(self.tab7, text="0", textvariable=self.node3_Xvalue_String, width=4,
                                           font=txtfont)
        self.node3_Yvalue_Label = tk.Label(self.tab7, text="0", textvariable=self.node3_Yvalue_String, width=4,
                                           font=txtfont)
        self.node3_Zvalue_Label = tk.Label(self.tab7, text="0", textvariable=self.node3_Zvalue_String, width=4,
                                           font=txtfont)
        self.node3_Xvalue_Label.grid(row=6, column=1)
        self.node3_Yvalue_Label.grid(row=6, column=3)
        self.node3_Zvalue_Label.grid(row=6, column=5)
        self.node3_Xvalue_String.set(0)
        self.node3_Yvalue_String.set(0)
        self.node3_Zvalue_String.set(0)

        self.node3_time_String = tk.StringVar()
        tk.Label(self.tab7, text="测试时间:", font=txtfont).grid(row=7, column=0, sticky=tk.W)
        self.node3_time_Label = tk.Label(self.tab7, text="0", textvariable=self.node3_time_String, width=26,
                                         font=txtfont)
        self.node3_time_Label.grid(row=7, column=1, sticky=tk.W, columnspan=3)
        self.node3_time_String.set(0)

        self.node4_num_String = tk.StringVar()
        tk.Label(self.tab7, text="节点编号:", font=txtfont).grid(row=4, column=7, sticky=tk.W)
        self.node4_num_Label = tk.Label(self.tab7, text="0", textvariable=self.node4_num_String, width=8, font=txtfont)
        self.node4_num_Label.grid(row=4, column=8, sticky=tk.W)
        self.node4_num_String.set(0)

        self.node4_Xmiddle_String = tk.StringVar()
        self.node4_Ymiddle_String = tk.StringVar()
        self.node4_Zmiddle_String = tk.StringVar()
        tk.Label(self.tab7, text="标定值:X:", font=txtfont).grid(row=5, column=7, sticky=tk.W)
        tk.Label(self.tab7, text="Y:", font=txtfont).grid(row=5, column=9)
        tk.Label(self.tab7, text="Z:", font=txtfont).grid(row=5, column=11)
        self.node4_Xmiddle_Label = tk.Label(self.tab7, text="0", textvariable=self.node4_Xmiddle_String, width=4,
                                            font=txtfont)
        self.node4_Ymiddle_Label = tk.Label(self.tab7, text="0", textvariable=self.node4_Ymiddle_String, width=4,
                                            font=txtfont)
        self.node4_Zmiddle_Label = tk.Label(self.tab7, text="0", textvariable=self.node4_Zmiddle_String, width=4,
                                            font=txtfont)
        self.node4_Xmiddle_Label.grid(row=5, column=8)
        self.node4_Ymiddle_Label.grid(row=5, column=10)
        self.node4_Zmiddle_Label.grid(row=5, column=12)
        self.node4_Xmiddle_String.set(0)
        self.node4_Ymiddle_String.set(0)
        self.node4_Zmiddle_String.set(0)

        self.node4_Xvalue_String = tk.StringVar()
        self.node4_Yvalue_String = tk.StringVar()
        self.node4_Zvalue_String = tk.StringVar()
        tk.Label(self.tab7, text="当前值:X:", font=txtfont).grid(row=6, column=7, sticky=tk.W)
        tk.Label(self.tab7, text="Y:", font=txtfont).grid(row=6, column=9)
        tk.Label(self.tab7, text="Z:", font=txtfont).grid(row=6, column=11)
        self.node4_Xvalue_Label = tk.Label(self.tab7, text="0", textvariable=self.node4_Xvalue_String, width=4,
                                           font=txtfont)
        self.node4_Yvalue_Label = tk.Label(self.tab7, text="0", textvariable=self.node4_Yvalue_String, width=4,
                                           font=txtfont)
        self.node4_Zvalue_Label = tk.Label(self.tab7, text="0", textvariable=self.node4_Zvalue_String, width=4,
                                           font=txtfont)
        self.node4_Xvalue_Label.grid(row=6, column=8)
        self.node4_Yvalue_Label.grid(row=6, column=10)
        self.node4_Zvalue_Label.grid(row=6, column=12)
        self.node4_Xvalue_String.set(0)
        self.node4_Yvalue_String.set(0)
        self.node4_Zvalue_String.set(0)

        self.node4_time_String = tk.StringVar()
        tk.Label(self.tab7, text="测试时间:", font=txtfont).grid(row=7, column=7, sticky=tk.W)
        self.node4_time_Label = tk.Label(self.tab7, text="0", textvariable=self.node4_time_String, width=26,
                                         font=txtfont)
        self.node4_time_Label.grid(row=7, column=8, sticky=tk.W, columnspan=3)
        self.node4_time_String.set(0)

    def updateTempLabel(self):
        '''
        Parameter：

        Function：
                               更新温漂测试标题
        :type self:
        Autor:xiaoxiami 2015.11.29
        Others：
        '''

        if self.temp_node1_middle_flag <= 10 and self.menu.uartform.temperaturethread.value_buf[0][0] * \
                self.menu.uartform.temperaturethread.value_buf[0][1] * \
                self.menu.uartform.temperaturethread.value_buf[0][2] != 0:
            self.temp_node1_middle_flag += 1
            self.node1_starttime = int(time.time())
            self.node1_num_String.set(self.menu.uartform.temperaturethread.value_buf[0][3])
            self.node1_Xmiddle_String.set(self.menu.uartform.temperaturethread.value_buf[0][0])
            self.node1_Ymiddle_String.set(self.menu.uartform.temperaturethread.value_buf[0][1])
            self.node1_Zmiddle_String.set(self.menu.uartform.temperaturethread.value_buf[0][2])
        if self.temp_node2_middle_flag <= 10 and self.menu.uartform.temperaturethread.value_buf[1][0] * \
                self.menu.uartform.temperaturethread.value_buf[1][1] * \
                self.menu.uartform.temperaturethread.value_buf[1][2] != 0:
            self.temp_node2_middle_flag += 1
            self.node2_starttime = int(time.time())
            self.node2_num_String.set(self.menu.uartform.temperaturethread.value_buf[1][3])
            self.node2_Xmiddle_String.set(self.menu.uartform.temperaturethread.value_buf[1][0])
            self.node2_Ymiddle_String.set(self.menu.uartform.temperaturethread.value_buf[1][1])
            self.node2_Zmiddle_String.set(self.menu.uartform.temperaturethread.value_buf[1][2])
        if self.temp_node3_middle_flag <= 10 and self.menu.uartform.temperaturethread.value_buf[2][0] * \
                self.menu.uartform.temperaturethread.value_buf[2][1] * \
                self.menu.uartform.temperaturethread.value_buf[2][2] != 0:
            self.temp_node3_middle_flag += 1
            self.node3_starttime = int(time.time())
            self.node3_num_String.set(self.menu.uartform.temperaturethread.value_buf[2][3])
            self.node3_Xmiddle_String.set(self.menu.uartform.temperaturethread.value_buf[2][0])
            self.node3_Ymiddle_String.set(self.menu.uartform.temperaturethread.value_buf[2][1])
            self.node3_Zmiddle_String.set(self.menu.uartform.temperaturethread.value_buf[2][2])
        if self.temp_node4_middle_flag <= 10 and self.menu.uartform.temperaturethread.value_buf[3][0] * \
                self.menu.uartform.temperaturethread.value_buf[3][1] * \
                self.menu.uartform.temperaturethread.value_buf[3][2] != 0:
            self.temp_node4_middle_flag += 1
            self.node4_starttime = int(time.time())
            self.node4_num_String.set(self.menu.uartform.temperaturethread.value_buf[3][3])
            self.node4_Xmiddle_String.set(self.menu.uartform.temperaturethread.value_buf[3][0])
            self.node4_Ymiddle_String.set(self.menu.uartform.temperaturethread.value_buf[3][1])
            self.node4_Zmiddle_String.set(self.menu.uartform.temperaturethread.value_buf[3][2])
        if self.temp_node1_middle_flag > 9:
            self.node1_num_String.set(self.menu.uartform.temperaturethread.value_buf[0][3])
            self.node1_Xvalue_String.set(self.menu.uartform.temperaturethread.value_buf[0][0])
            self.node1_Yvalue_String.set(self.menu.uartform.temperaturethread.value_buf[0][1])
            self.node1_Zvalue_String.set(self.menu.uartform.temperaturethread.value_buf[0][2])
            self.node1_time_String.set(self.timeFormat(int(time.time()) - self.node1_starttime))
        if self.temp_node2_middle_flag > 9:
            self.node2_num_String.set(self.menu.uartform.temperaturethread.value_buf[1][3])
            self.node2_Xvalue_String.set(self.menu.uartform.temperaturethread.value_buf[1][0])
            self.node2_Yvalue_String.set(self.menu.uartform.temperaturethread.value_buf[1][1])
            self.node2_Zvalue_String.set(self.menu.uartform.temperaturethread.value_buf[1][2])
            self.node2_time_String.set(self.timeFormat(int(time.time()) - self.node2_starttime))
        if self.temp_node3_middle_flag > 9:
            self.node3_num_String.set(self.menu.uartform.temperaturethread.value_buf[2][3])
            self.node3_Xvalue_String.set(self.menu.uartform.temperaturethread.value_buf[2][0])
            self.node3_Yvalue_String.set(self.menu.uartform.temperaturethread.value_buf[2][1])
            self.node3_Zvalue_String.set(self.menu.uartform.temperaturethread.value_buf[2][2])
            self.node3_time_String.set(self.timeFormat(int(time.time()) - self.node3_starttime))
        if self.temp_node4_middle_flag > 9:
            self.node4_num_String.set(self.menu.uartform.temperaturethread.value_buf[3][3])
            self.node4_Xvalue_String.set(self.menu.uartform.temperaturethread.value_buf[3][0])
            self.node4_Yvalue_String.set(self.menu.uartform.temperaturethread.value_buf[3][1])
            self.node4_Zvalue_String.set(self.menu.uartform.temperaturethread.value_buf[3][2])
            self.node4_time_String.set(self.timeFormat(int(time.time()) - self.node4_starttime))
        self.node1_num_Label.after(1000, self.updateTempLabel)

    def timeFormat(self, seconds):
        '''
        Parameter：

        Function：
                               格式化时间
        :type self:
        Autor:xiaoxiami 2015.11.29
        Others：
        '''
        if seconds < 60:
            return str(seconds) + " 秒"
        elif seconds < 3600:
            return str(seconds / 60) + " 分 " + str(seconds % 60) + " 秒"
        elif seconds < 86400:
            hour = seconds / 3600
            seconds %= 3600
            minutes = seconds / 60
            return str(hour) + " 小时 " + str(minutes) + " 分 " + str(seconds % 60) + " 秒"
        else:
            days = seconds / 86400
            seconds %= 86400
            hours = seconds / 3600
            seconds %= 3600
            minutes = seconds / 60
            return str(days) + " 天 " + str(hours) + " 小时 " + str(minutes) + " 分 " + str(seconds % 60) + " 秒"


class StatusBar(ttk.Frame):
    def __init__(self, master):
        ttk.Frame.__init__(self, master)
        self.label = ttk.Label(self, relief='sunken', anchor='w')
        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=1)
        self.rowconfigure(0, weight=1)
        self.label.grid(row=0, column=0, sticky=tk.W + tk.E + tk.N + tk.S)

        self.labelicon = ttk.Label(self, relief='sunken', anchor='w')
        self.labelicon.grid(row=0, column=1, sticky=tk.W + tk.E + tk.N + tk.S)

    def setstatus(self, defformat, *args):
        '''
        Parameter：
                                参数调用方法同print
        Function：
                                更新右侧状态栏显示的数据
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.label.config(text=defformat % args)

    def setdata(self, defformat, *args):
        '''
        Parameter：
                                参数调用方法同print
        Function：
                                更新左侧状态栏显示的数据
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.labelicon.config(text=defformat % args)

    def clear(self):
        '''
        Parameter：
            none
        Function：
                                清空状态栏
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.label.config(text="")
        self.label.update_idletasks()


if __name__ == '__main__':
    root = MainRoot()
    root.rootmenu.show()
    root.ShowData()  # 数据显示
    root.NetStatus()  # 网络状态
    root.StopStatus()  # 停车状态
    root.NetSniffer()  # 网络抓包
    root.SysStatus()  # 状态栏
    root.Command()  # 命令
    root.QOSTest()  # 链路测试
    root.Identify()  # 绘图
    root.tempTest()  # 温度测试
    root.algorithm()  # 算法测试
    root.mainloop()
