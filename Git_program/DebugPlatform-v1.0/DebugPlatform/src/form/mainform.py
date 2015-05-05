# coding=utf-8
import Tkinter as tk
import ttk
import uart
import serial
import mythread
import tkMessageBox as tkmes
import os
import time
import tkFont
class MainRoot(tk.Tk):
    """Container for all frames within the application"""
    
    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)
        
        # initialize menu
        self.rootmenu = MenuBar(self)
        self.config(menu=self.rootmenu)
        self.title("小虾米")

        # 居中显示
#         self.update() # update window ,must do
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
    def ShowData(self):
        self.appFrame.ShowData() 
         
    def NetStatus(self):
        self.appFrame.NetStatus()
        
    def StopStatus(self):
        self.appFrame.StopStatus()
    
    def NetSniffer(self):
        self.appFrame.NetSniffer()
        
    def SysStatus(self):
        self.status = StatusBar(self)
        self.status.pack(side='bottom', fill='x')

class CarStopRoot(tk.Tk):
    def __init__(self, *args, **kwargs):
        self.locat = 0
        self.carnum = 0
        self.datamode = 0
        self.updatamode = 1
    def show(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)
        
        self.locat = ""
        self.title("停车设置")
#         self.geometry('400x400')
        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=1)
        self.rowconfigure(0, weight=1) 
        self.rowconfigure(1, weight=1)
        self.rowconfigure(2, weight=1)
        self.rowconfigure(4, weight=1)
        self.rowconfigure(5, weight=1)
        self.rowconfigure(6, weight=1)
        
        carnumlabel = tk.Label(self, text="车位数量:")
        carnumlabel.grid(sticky=tk.W)
        self.carnumspinbox = tk.Spinbox(self, from_=4, to=50, width=10)

        self.carnumspinbox.grid(row=0, column=1)
        carlolabel = tk.Label(self, text="车位位置:")
        carlolabel.grid(row=2, column=0,sticky=tk.W)
        self.carlocomb = ttk.Combobox(self, width=10)
        self.carlocomb['value'] = ("方兴大厦", "其他")
        self.carlocomb.set("方兴大厦")
        self.carlocomb.grid(row=2, column=1,sticky=tk.W)
        
        tk.Label(self, text="数据上传:").grid(row=3, column=0)
        self.updatamodecomb = ttk.Combobox(self,width=10)
        self.updatamodecomb.grid(row=3,column=1)
        self.updatamodecomb['value']=("开启","不开启")
        self.updatamodecomb.set("不开启")
        
        tk.Label(self, text="数据源:").grid(row=4, column=0)
        self.datamodecomb = ttk.Combobox(self,width=10)
        self.datamodecomb.grid(row=4,column=1)
        self.datamodecomb['value']=("串口数据","网络数据")
        self.datamodecomb.set("串口数据")
        ttk.Button(self, text="确认", command=self.carstopconfirm).grid(row=5, column=0, columnspan=2)
      
    def nothing(self):
        pass  
    def carstopconfirm(self):
        self.locat = self.carlocomb.current()
        self.carnum = int(self.carnumspinbox.get())
        self.datamode = self.datamodecomb.current()
        self.updatamode = self.updatamodecomb.current()
        global root
        root.appFrame.DrawParking()
        try:
            self.destroy()
        except:
            pass
        
class MenuBar(tk.Menu):
    portlistsort = []
    def __init__(self, parent):
        tk.Menu.__init__(self, parent)
        self.carstop = CarStopRoot()
        filemenu = tk.Menu(self, tearoff=False)
        self.add_cascade(label="选项", underline=0, menu=filemenu)
        filemenu.add_command(label="串口设置...", command=self.Uartettings)
        
        filemenu.add_command(label="网络设置...", command=self.netsettings)
        
        filemenu.add_command(label="停车设置...", command=self.carstopsettings)

        filemenu.add_command(label="退出", underline=1, command=self.quit)

        helpmenu = tk.Menu(self, tearoff=False)
        self.add_cascade(label="帮助", menu=helpmenu)
        helpmenu.add_command(label="完善中", command=self.callback)

    def callback(self):
        print "called the callback!"
    
    def Uartettings(self):
        self.portlistsort = []
        uartportlist = uart.GetSerialPorts()
        uartportlist.enumerate_serial_ports()
        for portnum in range(len(uartportlist.portList)):
            self.portlistsort.append(str(uartportlist.portList[portnum][1]))
        self.portlistsort = sorted(self.portlistsort)
        global root
        self.uartform = UartRoot(comnum=self.portlistsort, frame=root)
        self.uartform.protocol("WM_DELETE_WINDOW", self.uart_shutdown_ttk_repeat)  # 防止退出报错
        self.uartform.mainloop()
    
    def netsettings(self):
        pass
    
    def carstopsettings(self):
        self.carstop.show()
        self.carstop.protocol("WM_DELETE_WINDOW", self.carstop_shutdown_ttk_repeat)  # 防止退出报错
        self.carstop.mainloop()
    # 防止退出报错
    def uart_shutdown_ttk_repeat(self):
        self.uartform.eval('::ttk::CancelRepeat')
        self.uartform.destroy()
    
    def carstop_shutdown_ttk_repeat(self):
        self.carstop.eval('::ttk::CancelRepeat')
        self.carstop.destroy()

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
       
    def setstatus(self, format, *args):
        self.label.config(text=format % args)
#         self.label.update_idletasks()
    
    def setdata(self, format, *args):
        self.labelicon.config(text=format % args)

    def clear(self):
        self.label.config(text="")
        self.label.update_idletasks()

class Application(ttk.Notebook):
    def __init__(self, root):
        ttk.Notebook.__init__(self, root)
        
        
        self.tab1 = ttk.Frame(self)
        self.tab2 = ttk.Frame(self)
        self.tab3 = ttk.Frame(self)
        self.tab4 = ttk.Frame(self)

        self.add(self.tab1, text="停车状态")
        self.add(self.tab2, text="网络拓扑")
        self.add(self.tab3, text="数据显示")
        self.add(self.tab4, text="网络抓包")
        self.topline = []
        self.bottomline = []
        self.carnum = 0
        self.width = 1320
        self.height = 600
        self.carmove = []
        self.stoptext = []
        self.loacname=[]
        self.front = tkFont.Font(size=50, family="黑体")
        
    def StopStatus(self):             
        self.canvas = tk.Canvas(self.tab1, width=self.width, height=self.height)
        self.canvas.grid(sticky=tk.W + tk.E)
        self.canvas.create_line(0, self.height / 2, self.width, self.height / 2, fill='black', dash=(100, 80), width=5)
        self.canvas.create_line(0, self.height / 3, self.width, self.height / 3, fill='black', width=5)
        self.canvas.create_line(0, self.height / 3 * 2, self.width, self.height / 3 * 2, fill='black', width=5)
    
    def DrawParking(self):
        global root
        self.carnum = root.rootmenu.carstop.carnum

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
        
        self.loacname.append(self.canvas.create_text(self.width/2,self.height/2,text = root.rootmenu.carstop.carlocomb.get(),font=tkFont.Font(size=50,family="黑体"),fill='yellow'))

        if self.carnum % 2 == 0:
            topnum = self.carnum / 2
            bottomnum = self.carnum / 2
        else:
            topnum = (self.carnum + 1) / 2
            bottomnum = (self.carnum - 1) / 2
        everytopx = self.width / topnum + 1
        everybottomx = self.width / bottomnum + 1
        for i in range(1, topnum):
            self.topline.append(self.canvas.create_line(everytopx * i, 0, everytopx * i, self.height / 3, fill='black', width=5))
            
        for i in range(1, bottomnum):
            self.bottomline.append(self.canvas.create_line(everybottomx * i, self.height / 3 * 2, everybottomx * i, self.height, fill='black', width=5))
                
    def stopcar(self, move):
        count = 0
        if len(self.stoptext) > 0:
            for text in self.stoptext:
                self.canvas.delete(text)
#         for i in range(self.carnum):
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
            bottomnum = topnum
        else:
            topwidth = self.width / ((self.carnum + 1) / 2)
            bottomwidth = self.width / ((self.carnum - 1) / 2)
            topnum = (self.carnum + 1) / 2
            bottomnum = (self.carnum - 1) / 2
        buf = move.split(",")
        self.carmove=[]
        for i in buf:
            self.carmove.append(i.split("|"))
        for num, act in self.carmove:
            if count < topnum:
                if act == '1':
                    self.stoptext.append(self.canvas.create_text(count * topwidth + topwidth / 2, self.height / 6, text=num + ":\n已停车", font=self.front, fill='red'))
                
                else:
                    self.stoptext.append(self.canvas.create_text(count * topwidth + topwidth / 2, self.height / 6, text=num + ":\n未停车", font=self.front, fil='green'))
            else:
                if act == '1':
                    self.stoptext.append(self.canvas.create_text((count - topnum) * bottomwidth + bottomwidth / 2, self.height - self.height / 6, text=num + ":\n已停车", font=self.front, fill='red'))
                else:
                    self.stoptext.append(self.canvas.create_text((count - topnum) * bottomwidth + bottomwidth / 2, self.height - self.height / 6, text=num + ":\n未停车", font=self.front, fill='green'))        
            count = count + 1 
        
    def NetStatus(self):
        label2 = ttk.Label(self.tab2, text="网络拓扑").grid()
    
    def ShowData(self):
        
        self.tab3.rowconfigure(0, weight=4)
        self.tab3.rowconfigure(1, weight=1)
        self.tab3.columnconfigure(0, weight=1)
        self.tab3.columnconfigure(1, weight=1)
        self.tab3.columnconfigure(2, weight=1)
        self.tab3.columnconfigure(3, weight=1)
#         接收frame        
        receivegroup = tk.LabelFrame(self.tab3, text="接收区")
        receivegroup.grid(row=0, column=0, columnspan=3, sticky=tk.N + tk.S + tk.E + tk.W)
#         停止键
        stopbutton = ttk.Button(self.tab3, text='暂停', command=self.pause)
        stopbutton.grid(row=1, column=1, sticky=tk.W)
#         继续键
        runbutton = ttk.Button(self.tab3, text='继续', command=self.restart)
        runbutton.grid(row=1, column=0, sticky=tk.E)
#         打开键
        openbutton = ttk.Button(self.tab3, text='打开txt数据', command=self.opentxt)
        openbutton.grid(row=1, column=1, sticky=tk.E)
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
        sendgroup.grid(row=0, column=3, columnspan=2, sticky=tk.N + tk.S + tk.E + tk.W)
#         发送键
        sendbutton = ttk.Button(self.tab3, text='发送', command=self.SendData)
        sendbutton.grid(row=1, column=3, sticky=tk.E)
#         发送输入框
        sendgroup.rowconfigure(0, weight=1)
        sendgroup.columnconfigure(0, weight=1)
        inputbuf = tk.Entry(self.tab3, width=20)
        inputbuf.grid(row=1, column=3, sticky=tk.W)
#         发送显示区
        
        inputtext = tk.Text(sendgroup)
        inputtext.grid(row=0, column=0, sticky=tk.N + tk.S + tk.E + tk.W)
        inputsb = tk.Scrollbar(inputtext)
        inputsb.pack(side=tk.RIGHT, fill=tk.Y)
        inputsb.config(command=inputtext.yview)
        inputtext.config(yscrollcommand=inputsb.set)
        
#         速率选择
        speedgroup = tk.LabelFrame(self.tab3, text="接收速率选择")
        speedgroup.grid(row=1, column=2, columnspan=1, sticky=tk.W)
        speedgroup.rowconfigure(0)
        speedgroup.rowconfigure(1)
        
        self.radiovalue = tk.IntVar()
        self.radiovalue.set(0)     
        tk.Radiobutton(speedgroup, variable=self.radiovalue, text='低速：接收区显示数据', value=0).grid()
        tk.Radiobutton(speedgroup, variable=self.radiovalue, text='高速：数据存储在txt中', value=1).grid()

#         inputlb = tk.Listbox(sendgroup)
#         inputlb.grid(row=0, column=0,sticky=tk.N+tk.S+tk.E+tk.W)

    def NetSniffer(self):
        label4 = ttk.Label(self.tab4, text="网络抓包").grid()
    
    def SendData(self):
        print self.radiovalue.get()
    
    def pause(self):
        global uartthread
        try:
            uartthread.thread_stop = True
        except NameError:
            tkmes.showwarning("错误", "串口未启动!")
  
    def restart(self):
        global uartthread
        try:
            uartthread.thread_stop = False
            uartthread.openfile()
        except NameError:
            tkmes.showwarning("错误", "串口未启动!")
        
    def updatetext(self, chars):
        try:
            self.receivetext.insert(1.0, chars)
        except:
            pass
    
    def opentxt(self):
        global uartthread
        self.pause()
        try:
            uartthread.clsoefile()
            os.startfile(uartthread.filename)
        except:
            pass
         
opened_uart = []
class UartRoot(tk.Tk):
    """Container for all frames within the application"""
    
    errtext = '没有错'
    def __init__(self, comnum, frame, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)
        self.frame = frame
        self.title("串口配置")
        self.geometry('200x400')
        ttk.Label(self, text="串口号:", padding=5).grid(row=0)
        self.comnumbox = ttk.Combobox(self, width=10, exportselection=0)
        self.comnumbox.bind("<<ComboboxSelected>>", self.IsOpen)
#         comnum.append('com2')
        self.comnumbox['value'] = comnum
        if len(comnum) > 0:
            self.comnumbox.set(comnum[0])
        else:
            self.comnumbox.set('未发现串口')
        self.comnumbox.grid(row=0, column=1)
        
        ttk.Label(self, text="波特率:", padding=5).grid(row=1)
        bordrate = ['9600', '14400', '19200', '28800', '38400', '56000', '57600', '115200']
        self.bordratebox = ttk.Combobox(self, width=10)
        self.bordratebox['value'] = bordrate
        self.bordratebox.set('115200')
        self.bordratebox.grid(row=1, column=1)
        
        ttk.Label(self, text="数据位:", padding=5).grid(row=2)
        databit = ['8bit', '7bit']
        self.databitbox = ttk.Combobox(self, width=10)
        self.databitbox['value'] = databit
        self.databitbox.set('8bit')
        self.databitbox.grid(row=2, column=1)
        
        ttk.Label(self, text="停止位:", padding=5).grid(row=3)
        stopbit = ['1bit', '2bit']
        self.stopbitbox = ttk.Combobox(self, width=10)
        self.stopbitbox['value'] = stopbit
        self.stopbitbox.set('1bit')
        self.stopbitbox.grid(row=3, column=1)
        
        
#         s = ttk.Style()
#         s.configure('TButton',
# #         background='black',
# #         foreground='white',
#         highlightthickness='10',
#         font=('黑体', 14))
#         s.map('TButton',
#         foreground=[('disabled', 'yellow'),('pressed', 'red'), ('active', 'blue')],
#         background=[('disabled', 'magenta'),('pressed', '!focus', 'cyan'),('active', 'green')],
#         highlightcolor=[('focus', 'green'),('!focus', 'red')],
#         relief=[('pressed', 'groove'),('!pressed', 'ridge')]
#         )
        
        uartopenbutton = ttk.Button(self, text='打开串口', command=self.OpenUart)
        uartopenbutton.grid(row=4, column=1)
        uartclosebutton = ttk.Button(self, text='关闭串口', command=self.CloseUart)
        uartclosebutton.grid(row=5, column=1)

        
        self.uartstatus = tk.Canvas(self, width=20, height=20, background='black')
        self.statusrec = self.uartstatus.create_rectangle(0, 0, 20, 20, fill='red')      
        self.uartstatus.grid(row=5, column=0)
        self.IsOpen(0)
        global txtfilename
        txtfilname = time.strftime('%Y-%m-%d_%H-%M-%S', time.localtime(time.time())) + '.txt'
        self.txtfilname = txtfilname
    
    def OpenUart(self):
#         mainform.root.status.set("%s",'sdf')
        global uartthread
        try:
            uartthread = mythread.myThread(rootframe=root, filename=self.txtfilname, threadID=1, name='uart', port=self.comnumbox.get(), baud=self.bordratebox.get())
            uartthread.setDaemon(True)
            uartthread.start()
            opened_uart.append(self.comnumbox.get())
            self.IsOpen(0)
            uartthread.thread_stop = False
            self.frame.status.setstatus('%s', self.comnumbox.get() + '已打开')
            
        except serial.SerialException, error:
            self.errtext = str(error)
            text = tk.Label(self, text="串口被占用！")
            text.grid(row=7, column=1)
            print str(error)

    def CloseUart(self):
        global uartthread
        try:
            if self.comnumbox.get() in opened_uart:
                opened_uart.remove(self.comnumbox.get())
            self.IsOpen(0)
            uartthread.thread_stop = True
            uartthread.closeuart()
            self.frame.status.setstatus('%s', self.comnumbox.get() + '已关闭')
        except NameError:
            self.frame.status.setstatus('%s', '串口未打开')

    def IsOpen(self, event):
        comnunm = self.comnumbox.get()
        if comnunm in opened_uart:
            self.uartstatus.itemconfig(self.statusrec, fill='green')
        else:
            self.uartstatus.itemconfig(self.statusrec, fill='red')



if __name__ == '__main__':
    global root
    root = MainRoot()
    root.ShowData()  # 数据显示
    root.NetStatus()  # 网络状态
    root.StopStatus()  # 停车状态
    root.NetSniffer()  # 网络抓包
    root.SysStatus()  # 状态栏

    root.mainloop()

    