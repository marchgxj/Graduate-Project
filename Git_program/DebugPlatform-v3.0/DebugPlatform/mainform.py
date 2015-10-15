# coding=utf-8
import Tkinter as tk
import ttk
import uart
import tkMessageBox as tkmes
import os
import tkFont
import carstop
import time
import tkFileDialog
import matplot
import matplotold
import matplotoldanimate
import ctypes

__author__ = 'xiaoxiami'


class MainRoot(tk.Tk):
    """Container for all frames within the application"""

    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)

        # initialize menu
        self.rootmenu = MenuBar(self)
        self.config(menu=self.rootmenu)
        self.title("小虾米")

        # 居中显示
        curWidth = self.winfo_screenwidth()  # get current width
        curHeight = self.winfo_screenheight()  # get current height
        scnWidth = (curWidth - 1330) / 2
        scnHeight = (curHeight - 660) / 2  # get screen width and height
        # now generate configuration information
        tmpcnf = '%dx%d+%d+%d' % (1330, 660, scnWidth, scnHeight)

        self.geometry(tmpcnf)
        self.appcanvas = tk.Canvas(self)
        self.appFrame = Application(root=self)
        self.appFrame.pack(side='top', fill='both', expand='True')

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

    def NetSniffer(self):
        self.appFrame.DrawSensor()

    def SysStatus(self):
        self.status = StatusBar(self)
        self.status.pack(side='bottom', fill='x')

    def Identify(self):
        self.appFrame.Identify()
        pass


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
        filemenu.add_command(label="网络设置...", command=self.netsettings)
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

        global root

        self.uartform = uart.UartRoot(comnum=self.portlistsort, frame=root)

        self.root.appFrame.bindtab()
        self.uartform.showupdate()
        self.uartform.protocol("WM_DELETE_WINDOW", self.uart_shutdown_ttk_repeat)  # 防止退出报错
        '''删掉了mainloop'''
        self.uartform.mainloop()

    def netsettings(self):
        pass

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

        self.add(self.tab1, text="停车状态")
        self.add(self.tab2, text="网络拓扑")
        self.add(self.tab3, text="网络抓包")
        self.add(self.tab4, text="数据识别")
        self.add(self.tab5, text="控制命令")
        self.topline = []
        self.bottomline = []
        self.carnum = 0
        self.width = 1320
        self.height = 600
        self.carmove = []
        self.stoptext = []
        self.loacname = []
        self.front = tkFont.Font(size=50, family="黑体")

        self.root = root
        self.menu = root.rootmenu

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
        self.matplotopen = False
        self.datapath = ""
        self.matplotoldopen = False
        self.x1 = 0

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
        # self.menu.uartform.snifferthread.currenttab = self.index('current')
        # self.menu.uartform.identifythread.currenttab = self.index('current')
        # try:
        #     self.menu.uartform.snifferthread.currenttab = self.index('current')
        # except:
        #     print "7"
        # try:
        #     self.menu.uartform.identifythread.currenttab = self.index('current')
        # except:
        #     print "7"

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

    def stopcar(self, move):
        '''
        Parameter：
            move：传入的数据包格式如下： 1,0|2,1|3,0|4,0
        Function：
                                更新停车界面
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        count = 0
        try:
            if len(self.stoptext) > 0:
                for text in self.stoptext:
                    self.canvas.delete(text)
        except:
            print "5"
        # for i in range(self.carnum):
        #             self.stoptext.append(self.canvas.create_text(0,0))
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
        # bottomnum = (self.carnum - 1) / 2
        buf = move.split(",")
        self.carmove = []
        for i in buf:
            self.carmove.append(i.split("|"))
        for num, act in self.carmove:
            if count < topnum:
                if act == '1':
                    self.stoptext.append(
                        self.canvas.create_text(count * topwidth + topwidth / 2, self.height / 6, text=num + ":\n已停车",
                                                font=self.front, fill='red'))

                else:
                    self.stoptext.append(
                        self.canvas.create_text(count * topwidth + topwidth / 2, self.height / 6, text=num + ":\n未停车",
                                                font=self.front, fil='green'))
            else:
                if act == '1':
                    self.stoptext.append(self.canvas.create_text((count - topnum) * bottomwidth + bottomwidth / 2,
                                                                 self.height - self.height / 6, text=num + ":\n已停车",
                                                                 font=self.front, fill='red'))
                else:
                    self.stoptext.append(self.canvas.create_text((count - topnum) * bottomwidth + bottomwidth / 2,
                                                                 self.height - self.height / 6, text=num + ":\n未停车",
                                                                 font=self.front, fill='green'))
            count = count + 1

    def stopcaronce(self, move):
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
                    self.canvas.itemconfigure(('text' + str(num)), text=num + ":\n已停车", fill='red')
                elif act == '0':
                    self.canvas.itemconfigure(('text' + str(num)), text=num + ":\n未停车", fill='green')
                elif act == '2':
                    self.canvas.itemconfigure(('text' + str(num)), text=num + ":\n识别中", fill='blue')
                elif act == '253':
                    self.canvas.itemconfigure(('text' + str(num)), text=num + ":\n掉线", fill='yellow')
                elif act == '252':
                    self.canvas.itemconfigure(('text' + str(num)), text=num + ":\n电量低", fill='yellow')
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
        canv = tk.Canvas(self.tab2, width=1320, height=600, bg="white")
        canv.grid()

        canv.create_image(1320 / 2, 50, image=self.rooterimage)
        canv.create_image(100, 400, image=self.sensorimage)
        canv.create_line(1320 / 2, 50, 100, 400, fill='green', dash=(100, 80))
        canv.create_text(100, 480, text="01")
        canv.create_image(400, 400, image=self.sensorimage)
        canv.create_line(1320 / 2, 50, 400, 400, fill='green', dash=(100, 80))
        canv.create_text(400, 480, text="02")
        canv.create_image(800, 400, image=self.sensorimage)
        canv.create_line(1320 / 2, 50, 800, 400, fill='green', dash=(100, 80))
        canv.create_text(800, 480, text="03")
        canv.create_image(1200, 400, image=self.sensorimage)
        canv.create_line(1320 / 2, 50, 1200, 400, fill='red', dash=(100, 80))
        canv.create_text(1200, 480, text="04")

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
        self.receivetext.after(500,self.updatetext)
        if(self.tab == 2):
            self.receivetext.insert(1.0, self.menu.uartform.snifferthread.dataintextbuf)
            self.menu.uartform.snifferthread.dataintextbuf=""
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

        self.dataRPbutton = tk.Button(self.tab4, command=self.MatplotlibDrawing, background="red", text="显示图像")
        self.dataRPbutton.grid(row=1, column=3, sticky=tk.E)
        self.dataclearbutton = tk.Button(self.tab4, text="清屏", command=self.Cleardata)
        self.dataclearbutton.grid(row=1, column=3, padx=20, sticky=tk.W, )

        self.newtext = tk.Button(self.tab4, command=self.newtext, text="新文件")
        self.newtext.grid(row=1, column=4)

        self.datascale = ttk.Scale(self.tab4, orient=tk.HORIZONTAL, from_=1, to=10, command=self.Zoomcallback)
        self.datascale.grid(row=1, column=2, sticky=tk.W)
        self.datascalevalue = 5
        self.datascale.set(self.datascalevalue)
        self.datascalelabel = ttk.Label(self.tab4, text="5")
        self.datascalelabel.grid(row=1, column=2, sticky=tk.E)

        receivegroup = tk.LabelFrame(self.tab4, text="识别结果")
        receivegroup.grid(row=0, column=0, columnspan=3, sticky=tk.N + tk.S + tk.E + tk.W)

        receivegroup.rowconfigure(0, weight=1)
        receivegroup.columnconfigure(0, weight=1)

        self.datacavas = tk.Canvas(receivegroup)
        self.datacavas.grid(row=0, column=0, columnspan=2, sticky=tk.N + tk.S + tk.E + tk.W)
        self.cancassbx = tk.Scrollbar(self.datacavas, orient=tk.HORIZONTAL)
        self.cancassbx.pack(side=tk.BOTTOM, fill=tk.X)
        self.cancassbx.config(command=self.datacavas.xview)
        self.datacavas.config(xscrollcommand=self.cancassbx.set)

        # cancassby = tk.Scrollbar(self.datacavas)
        # cancassby.pack(side=tk.RIGHT, fill=tk.Y)
        # cancassby.config(command=self.datacavas.yview)
        # self.datacavas.config(yscrollcommand=cancassby.set)

        readgroup = tk.LabelFrame(self.tab4, text="数据读取")
        readgroup.grid(row=0, column=3, columnspan=3, sticky=tk.N + tk.S + tk.E + tk.W)


        for i in range(10):
            readgroup.rowconfigure(i, weight=1)
        for i in range(10):
            readgroup.columnconfigure(i, weight=1)

        rowandcloumn = 0

        self.XValueString = tk.StringVar()
        tk.Label(readgroup, text="XValue:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.XValueLabel = tk.Label(readgroup, text="0",textvariable=self.XValueString,width=4)
        self.XValueLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.XValueString.set(0)

        self.XMiddleString = tk.StringVar()
        tk.Label(readgroup, text="XMiddle:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.AD_middle_valueXLabel = tk.Label(readgroup, text="0",textvariable=self.XMiddleString,width=4)
        self.AD_middle_valueXLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.XMiddleString.set(0)

        self.YValueString = tk.StringVar()
        tk.Label(readgroup, text="YValue:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.YValueLabel = tk.Label(readgroup, text="0",textvariable=self.YValueString,width=4)
        self.YValueLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.YValueString.set(0)

        self.YMiddleString = tk.StringVar()
        tk.Label(readgroup, text="YMiddle:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.AD_middle_valueYLabel = tk.Label(readgroup, text="0",textvariable=self.YMiddleString,width=4)
        self.AD_middle_valueYLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.YMiddleString.set(0)

        self.ZvalueString = tk.StringVar()
        tk.Label(readgroup, text="ZValue:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.ZvalueLabel = tk.Label(readgroup, text="0",textvariable=self.ZvalueString,width=4)
        self.ZvalueLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.ZvalueString.set(0)

        self.ZMiddleString = tk.StringVar()
        tk.Label(readgroup, text="ZMiddle:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.AD_middle_valueZLabel = tk.Label(readgroup, text="0",textvariable=self.ZMiddleString,width=4)
        self.AD_middle_valueZLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.ZMiddleString.set(0)

        self.VarianceString = tk.StringVar()
        tk.Label(readgroup, text="Variance:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.VarianceMLabel = tk.Label(readgroup, text="0",textvariable=self.VarianceString,width=4)
        self.VarianceMLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.VarianceString.set(0)

        self.VarStateString = tk.StringVar()
        tk.Label(readgroup, text="VarState:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.VarStateLabel = tk.Label(readgroup, text="0",textvariable=self.VarStateString,width=4)
        self.VarStateLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.VarStateString.set(0)

        self.ExtremumString = tk.StringVar()
        tk.Label(readgroup, text="Extremum:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.ExtremumValueLabel = tk.Label(readgroup, text="0",textvariable=self.ExtremumString,width=4)
        self.ExtremumValueLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.ExtremumString.set(0)

        self.Ext_MiddleString = tk.StringVar()
        tk.Label(readgroup, text="Ext_Middle:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.ExtremumValueMiddleLabel = tk.Label(readgroup, text="0",textvariable=self.Ext_MiddleString,width=4)
        self.ExtremumValueMiddleLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.Ext_MiddleString.set(0)

        self.ExtStateString = tk.StringVar()
        tk.Label(readgroup, text="ExtState:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.ExtStateLabel = tk.Label(readgroup, text="0",textvariable=self.ExtStateString,width=4)
        self.ExtStateLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.ExtStateString.set(0)

        self.IntensityeString = tk.StringVar()
        tk.Label(readgroup, text="Intensity:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.IntensityLabel = tk.Label(readgroup, text="0",textvariable=self.IntensityeString,width=4)
        self.IntensityLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.IntensityeString.set(0)

        self.Int_MiddleString = tk.StringVar()
        tk.Label(readgroup, text="Int_Middle:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.IntensityMiddleLabel = tk.Label(readgroup, text="0",textvariable=self.Int_MiddleString,width=4)
        self.IntensityMiddleLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.Int_MiddleString.set(0)

        self.IntStateString = tk.StringVar()
        tk.Label(readgroup, text="IntState:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.IntStateLabel = tk.Label(readgroup, text="0",textvariable=self.IntStateString,width=4)
        self.IntStateLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.IntStateString.set(0)

        self.ResultString = tk.StringVar()
        tk.Label(readgroup, text="Result:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.ResultLabel = tk.Label(readgroup, text="0",textvariable=self.ResultString,width=4)
        self.ResultLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.ResultString.set(0)

        self.IntensityMinusString = tk.StringVar()
        tk.Label(readgroup, text="IntensityMinus:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.IntensityMinus = tk.Label(readgroup, text="0",textvariable=self.IntensityMinusString,width=4)
        self.IntensityMinus.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.IntensityMinusString.set(0)

        self.XAve_SlopString = tk.StringVar()
        tk.Label(readgroup, text="XAve_Slop:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.XAve_SlopLabel = tk.Label(readgroup, text="0",textvariable=self.XAve_SlopString,width=4)
        self.XAve_SlopLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.XAve_SlopString.set(0)

        self.YAve_SlopString = tk.StringVar()
        tk.Label(readgroup, text="YAve_Slop:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.YAve_SlopLabel = tk.Label(readgroup, text="0",textvariable=self.YAve_SlopString,width=4)
        self.YAve_SlopLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.YAve_SlopString.set(0)

        self.Side_ParkingString = tk.StringVar()
        tk.Label(readgroup, text="Side_Parking:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.Side_ParkingLabel = tk.Label(readgroup, text="0",textvariable=self.Side_ParkingString,width=4)
        self.Side_ParkingLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.Side_ParkingString.set(0)

        self.GMI_XvalueString = tk.StringVar()
        tk.Label(readgroup, text="GMI_Xvalue:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.GMI_XValueLabel = tk.Label(readgroup, text="0",textvariable=self.GMI_XvalueString,width=4)
        self.GMI_XValueLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.GMI_XvalueString.set(0)

        self.GMI_XvalueMiddleString = tk.StringVar()
        tk.Label(readgroup, text="GMI_XvalueM:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.GMI_XValueMiddleLabel = tk.Label(readgroup, text="0",textvariable=self.GMI_XvalueMiddleString,width=4)
        self.GMI_XValueMiddleLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.GMI_XvalueMiddleString.set(0)

        self.GMI_YvalueString = tk.StringVar()
        tk.Label(readgroup, text="GMI_Yvalue:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.GMI_YValueLabel = tk.Label(readgroup, text="0",textvariable=self.GMI_YvalueString,width=4)
        self.GMI_YValueLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.GMI_YvalueString.set(0)

        self.GMI_YvalueMiddleString = tk.StringVar()
        tk.Label(readgroup, text="GMI_YvalueM:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.GMI_YValueMiddleLabel = tk.Label(readgroup, text="0",textvariable=self.GMI_YvalueMiddleString,width=4)
        self.GMI_YValueMiddleLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.GMI_YvalueMiddleString.set(0)

        self.VoltageString = tk.StringVar()
        tk.Label(readgroup, text="Voltage:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.VoltageLabel = tk.Label(readgroup, text="0",textvariable=self.VoltageString,width=4)
        self.VoltageLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.VoltageString.set(0)

        self.TemperatureString = tk.StringVar()
        tk.Label(readgroup, text="Temperature:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.TemperatureLabel = tk.Label(readgroup, text="0",textvariable=self.TemperatureString,width=4)
        self.TemperatureLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.TemperatureString.set(0)

        self.XValue_ParkingString = tk.StringVar()
        tk.Label(readgroup, text="XValue_Parking:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.XValue_ParkingLabel = tk.Label(readgroup, text="0",textvariable=self.XValue_ParkingString,width=4)
        self.XValue_ParkingLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.XValue_ParkingString.set(0)

        self.YValue_ParkingString = tk.StringVar()
        tk.Label(readgroup, text="YValue_Parking:").grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.YValue_ParkingLabel = tk.Label(readgroup, text="0",textvariable=self.YValue_ParkingString,width=4)
        self.YValue_ParkingLabel.grid(row=rowandcloumn/10, column=rowandcloumn%10)
        rowandcloumn+=1
        self.YValue_ParkingString.set(0)

        # self.datatext = tk.Text(readgroup)
        # self.datatext.grid(row=0, column=0, sticky=tk.N + tk.S + tk.E + tk.W)

        # inputsb = tk.Scrollbar(self.datatext)
        # inputsb.pack(side=tk.RIGHT, fill=tk.Y)
        # inputsb.config(command=self.datatext.yview)
        # self.datatext.config(yscrollcommand=inputsb.set)

    def Cleardata(self):
        '''
        Parameter：

        Function：
                              清空识别界面
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        for v in self.canvasidentifyline:
            self.datacavas.delete(v)
        self.canvasidentifyline = []
        for v in self.canvasline:
            self.datacavas.delete(v)
        self.canvasline = []
        for v in self.canvasoval:
            self.datacavas.delete(v)
        self.canvasoval = []
        for v in self.admiddleline:
            self.datacavas.delete(v)
        self.admiddleline = []
        self.datacavas.config(scrollregion=(0, 0, 0, 512))
        self.datapath = ""

    def confirm(self):
        '''
        Parameter：

        Function：
                              输入新的文件名
        Autor:xiaoxiami 2015.9.05
        Others：
        '''
        self.filenameadd = self.entry.get()
        self.filenameinput.quit()

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
            self.filenameinput.mainloop()
            self.menu.uartform.identifythread.filename = '..\Data\\' + self.filenameadd + time.strftime(
                '%Y-%m-%d_%H-%M-%S', time.localtime(time.time())) + '.txt'

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
                self.dataRPbutton.configure(background="red", text="显示图像")
            else:
                self.identifythread.thread_stop = False
                self.dataRPbutton.configure(background="green", text="显示图像")
                self.scope = matplot.Scope(thread=self.menu.uartform.identifythread)
                self.scope.start()
        else:

            self.matplotanimate = matplotoldanimate.Scope(data=self.filedata, thread=self)
            self.matplotanimate.start()
            # self.DrawOldDataByMatplot(data=self.filedata)

    def Selectdata(self):
        '''
        Parameter：

        Function：
                              选择txt文件路径
        Autor:xiaoxiami 2015.8.30
        Others：默认打开目录为 F:\Graduate\Git_program\DebugPlatform-v3.0\Data
        '''

        self.datapath = tkFileDialog.askopenfilename(initialdir='..\Data')
        self.datapathentry.delete(0, tk.END)
        self.datapathentry.insert(1, self.datapath)
        self.zoomenable = 1
        try:
            self.data_file_object = open(self.datapath)
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
        # self.DrawOldDataByMatplot()

    def DrawOldDataByMatplot(self, data):
        '''
        Parameter：

        Function：
                              根据历史数据绘制图像
        Autor:xiaoxiami 2015.8.30
        Others：
        '''
        self.matplotoldopen = True
        self.DrawOldData = matplotold.ScopeOld(data)
        self.DrawOldData.show()

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
        for v in data:
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


        # 删除遗留图像
        for v in self.canvasidentifyline:
            self.datacavas.delete(v)
        self.canvasidentifyline = []
        for v in self.canvasline:
            self.datacavas.delete(v)
        self.canvasline = []
        for v in self.canvasoval:
            self.datacavas.delete(v)
        self.canvasoval = []
        for v in self.admiddleline:
            self.datacavas.delete(v)
        self.admiddleline = []

        x = 0
        ym = 0
        try:
            offset = int(self.datascale.get())
        except:
            print "offset error"
        self.datacavas.config(scrollregion=(0, 0, len(self.ExtState) * offset, 512))
        for v in self.ExtState:
            if (int(v) == 1):
                y = 512 - (int(2) * 30 + 10)
            elif (int(v) == 2):
                y = 512 - (int(1) * 30 + 10)
            else:
                y = 512 - (int(v) * 30 + 10)
            self.canvasline.append(self.datacavas.create_line(x, ym, x + offset, y, fill="blue"))
            x += offset
            ym = y
        x = 0
        ym = 0
        for v in self.VarState:
            if (int(v) == 1):
                y = 512 - (int(2) * 30 + 110)
            elif (int(v) == 2):
                y = 512 - (int(1) * 30 + 110)
            else:
                y = 512 - (int(v) * 30 + 110)
            self.canvasline.append(self.datacavas.create_line(x, ym, x + offset, y, fill="red"))
            x += offset
            ym = y
        x = 0
        ym = 0
        for v in self.IntState:
            if (int(v) == 1):
                y = 512 - (int(2) * 30 + 210)
            elif (int(v) == 2):
                y = 512 - (int(1) * 30 + 210)
            else:
                y = 512 - (int(v) * 30 + 210)
            self.canvasline.append(self.datacavas.create_line(x, ym, x + offset, y, fill="#FF9900"))
            x += offset
            ym = y
        x = 0
        ym = 0
        for v in self.Result:
            if (int(v) == 1):
                y = 512 - (int(2) * 30 + 310)
            elif (int(v) == 2):
                y = 512 - (int(1) * 30 + 310)
            else:
                y = 512 - (int(v) * 30 + 310)
            self.canvasline.append(self.datacavas.create_line(x, ym, x + offset, y, fill="#8B0A50"))
            x += offset
            ym = y

        self.datacavas.bind("<Button-1>", self.Showdetaildata)
        self.datacavas.bind("<B1-Motion>", self.Showdetaildata)
        self.datacavas.bind("<KeyPress>", self.keyxlim)

        self.datacavas.focus_set()

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
        self.XMiddleString.set(self.menu.uartform.identifythread.value[7])
        self.YMiddleString.set(self.menu.uartform.identifythread.value[8])
        self.IntensityeString.set(self.menu.uartform.identifythread.value[9])
        self.Int_MiddleString.set(self.menu.uartform.identifythread.value[10])
        self.IntensityMinusString.set(abs(self.menu.uartform.identifythread.value[9]-self.menu.uartform.identifythread.value[10]))
        self.IntStateString.set(self.menu.uartform.identifythread.value[11])
        self.ResultString.set(self.menu.uartform.identifythread.value[12])
        self.XAve_SlopString.set(ctypes.c_int16(self.menu.uartform.identifythread.value[13]).value)
        self.YAve_SlopString.set(ctypes.c_int16(self.menu.uartform.identifythread.value[14]).value)

        self.ZvalueString.set(self.menu.uartform.identifythread.value[16])
        self.GMI_XvalueString.set(self.menu.uartform.identifythread.value[17])
        self.GMI_YvalueString.set(self.menu.uartform.identifythread.value[18])
        self.GMI_XvalueMiddleString.set(self.menu.uartform.identifythread.value[19])
        self.GMI_YvalueMiddleString.set(self.menu.uartform.identifythread.value[20])
        self.VoltageString.set(self.menu.uartform.identifythread.value[21])
        self.TemperatureString.set(self.menu.uartform.identifythread.value[22])
        self.ZMiddleString.set(self.menu.uartform.identifythread.value[23])
        self.XValue_ParkingString.set(self.menu.uartform.identifythread.value[24])
        self.YValue_ParkingString.set(self.menu.uartform.identifythread.value[25])

        if(abs(self.menu.uartform.identifythread.value[0]-self.menu.uartform.identifythread.value[7])>100 and (self.menu.uartform.identifythread.value[2])<60):
            self.Side_ParkingString.set(1)
        else:
            self.Side_ParkingString.set(0)


        self.XValueLabel.after(50,self.updatelabel)
        # self.XValueString.set(value[0])
        # self.YValueString.set(value[1])
        # self.VarianceMLabel.config(text=str(value[2]))
        # self.AD_middle_valueXLabel.config(text=str(value[7]))
        # self.AD_middle_valueYLabel.config(text=str(value[8]))
        # self.ExtremumValueLabel.config(text=str(value[4]))
        # self.ExtremumValueMiddleLabel.config(text=str(value[3]))
        # self.IntensityLabel.config(text=str(value[9]))
        # self.IntensityMiddleLabel.config(text=str(value[10]))
        # self.IntStateLabel.config(text=str(value[11]))
        # self.ResultLabel.config(text=str(value[12]))
        # self.IntensityMinus.config(text=str(abs(int(value[9]) - int(value[10]))))
        # self.XAve_SlopLabel.config(text=str(value[13]))
        # self.YAve_SlopLabel.config(text=str(value[14]))

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
        offset = int(self.datascale.get())
        try:
            x1 = event.x + int(self.cancassbx.get()[0] * len(self.ExtState) * offset)
            self.x1 = x1
        except:
            x1  = self.x1
        self.xlim = x1 / offset
        if (x1 >= 0 and x1 <= len(self.ExtState) * offset):
            for v in self.cavasverticalline:
                self.datacavas.delete(v)
            self.cavasverticalline = []
            self.cavasverticalline.append(self.datacavas.create_line(x1, 0, x1, 512, fill="purple"))
            self.statusbar.setdata("%s", "Count:" + str(x1 / offset))
            # self.statusbar.setdata("%s","方差:" + str(self.VarState[x1 / offset]) + ",极值:" + str(self.ExtState[x1 / offset]))

            self.XValueString.set(self.XValue[x1 / offset])
            self.YValueString.set(self.YValue[x1 / offset])
            self.ZValueString.set(self.ZValue[x1 / offset])
            self.GMI_XvalueString.set(self.GMI_XValue[x1 / offset])
            self.GMI_YvalueString.set(self.GMI_YValue[x1 / offset])
            self.GMI_XvalueMiddleString.set(self.GMI_XValueM[x1 / offset])
            self.GMI_YvalueMiddleString.set(self.GMI_YValueM[x1 / offset])
            self.VarianceString.set(self.VarianceM[x1 / offset])

            self.XMiddleString.set(self.AD_middle_valueX[x1 / offset])
            self.YMiddleString.set(self.AD_middle_valueY[x1 / offset])
            self.ExtremumString.set(self.ExtremumValue[x1 / offset])
            self.Ext_MiddleString.set(self.ExtremumValueMiddle[x1 / offset])
            self.IntensityeString.set(self.Intensity[x1 / offset])
            self.Int_MiddleString.set(self.IntensityMiddle[x1 / offset])
            self.IntStateString.set(self.IntState[x1 / offset])
            self.ResultString.set(self.Result[x1 / offset])
            self.IntensityMinusString.set(abs(int(self.Intensity[x1 / offset]) - int(self.IntensityMiddle[x1 / offset])))
            self.XAve_SlopString.set(ctypes.c_int16(int(self.XAve_Slop[x1 / offset])))
            self.YAve_SlopString.set(ctypes.c_int16(int(self.YAve_Slop[x1 / offset])))
            if(abs(int(self.XValue[x1 / offset])-int(self.AD_middle_valueX[x1 / offset]))>100 and (int(self.VarianceM[x1 / offset]))<60):
                self.Side_ParkingString.set(1)
            # else:
                self.Side_ParkingString.set(0)


            # if(self.matplotoldopen == True):
            #     if(self.animateopen == 0):
            #         self.animateopen = 1
            #         self.matplotanimate = matplotoldanimate.Scope(data = self.filedata,thread = self)
            #         self.matplotanimate.start()
            # self.DrawOldData.set_xlim(x1/offset)

    def SendCommand(self):

        def cmdtext(cmd):
            address = cmdinput.get()

            try:
                self.menu.uartform.relaythread.SendCmd(address,cmd)
            except:
                tkmes.showerror("错误！","串口未打开，无法发送！")

        for i in range(6):
            self.tab5.columnconfigure(i, weight=1)
        phyaddtxtfont = tkFont.Font(size=20, family="黑体")
        tk.Label(self.tab5,text = "物理地址：",font = phyaddtxtfont).grid(row=0,column=1,sticky=tk.E)
        cmdinput = tk.Entry(self.tab5, width=15)
        cmdinput.grid(row=0,column=2,sticky=tk.W)
        buttonfont = tkFont.Font(size=16, family="黑体")
        cmdbutton1 = tk.Button(self.tab5,text="数据回传",command = lambda: cmdtext(1),font=buttonfont)
        cmdbutton2 = tk.Button(self.tab5,text="传感器校准",command = lambda: cmdtext(2),font=buttonfont)
        cmdbutton1.grid(row=1,column=0)
        cmdbutton2.grid(row=1,column=1)


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
    root.Command()  #命令
    root.Identify()
    root.mainloop()
