# coding=utf-8
import Tkinter as tk
import ttk
import uart
import tkMessageBox as tkmes
import os
import tkFont
import carstop

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
        
class MenuBar(tk.Menu):
    portlistsort = []
    def __init__(self, parent):
        tk.Menu.__init__(self, parent)
        self.carlocatecboxbuf='方兴大厦'
        self.updatamodecboxbuf ='开启'
        self.datamodecboxbuf = "串口数据"
        self.root = parent
        
    
    def show(self):
        
        self.carstoproot = carstop.CarStopRoot(rootframe = self.root)
        filemenu = tk.Menu(self, tearoff=False)
        self.add_cascade(label="选项", underline=0, menu=filemenu)
        filemenu.add_command(label="串口设置...", command=self.Uartettings)
        filemenu.add_command(label="网络设置...", command=self.netsettings)
        filemenu.add_command(label="停车设置...", command=self.carstopsettings)
        filemenu.add_command(label="退出", underline=1, command=self.quit)
        helpmenu = tk.Menu(self, tearoff=False)
        self.add_cascade(label="帮助", menu=helpmenu)
        helpmenu.add_command(label="完善中", command=self.callback)
        self.bordratecboxbuf='115200'
        self.datasourcecboxbuf='sniffer'
        self.datasourcecboxvalue = 5
        self.opened_uart=[]
           

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
        self.uartform = uart.UartRoot(comnum=self.portlistsort, frame=root)
        self.uartform.showupdate()
        self.uartform.protocol("WM_DELETE_WINDOW", self.uart_shutdown_ttk_repeat)  # 防止退出报错
        '''删掉了mainloop'''
        self.uartform.mainloop()
    
    def netsettings(self):
        pass
    
    def carstopsettings(self):
        
        self.carstoproot.show()
        self.carstoproot.showupdata()
        self.carnum = self.carstoproot.carnum
        self.carstoproot.protocol("WM_DELETE_WINDOW", self.carstop_shutdown_ttk_repeat)  # 防止退出报错
        '''删掉了mainloop'''
        self.carstoproot.mainloop()
        
    # 防止退出报错
    def uart_shutdown_ttk_repeat(self):
        self.uartform.eval('::ttk::CancelRepeat')
        self.uartform.destroy()
    
    def carstop_shutdown_ttk_repeat(self):
        self.carstoproot.eval('::ttk::CancelRepeat')
        self.carstoproot.destroy()

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
        self.label.config(text=defformat % args)
#         self.label.update_idletasks()
    
    def setdata(self, defformat, *args):
        self.labelicon.config(text=defformat % args)

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
        
        self.root = root
        self.menu = root.rootmenu
        
        self.bind("<<NotebookTabChanged>>", self.updatetab)
    
    def updatetab(self,event):
        try:
            self.menu.uartform.snifferthread.currenttab = self.index('current')
        except:
            print "6"
        
    def StopStatus(self):             
        self.canvas = tk.Canvas(self.tab1, width=self.width, height=self.height)
        self.canvas.grid(sticky=tk.W + tk.E)
        self.canvas.create_line(0, self.height / 2, self.width, self.height / 2, fill='black', dash=(100, 80), width=5)
        self.canvas.create_line(0, self.height / 3, self.width, self.height / 3, fill='black', width=5)
        self.canvas.create_line(0, self.height / 3 * 2, self.width, self.height / 3 * 2, fill='black', width=5)
    
    def DrawParking(self):
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
        
        self.loacname.append(self.canvas.create_text(self.width/2,self.height/2,text = root.rootmenu.carlocatecboxbuf,font=tkFont.Font(size=50,family="黑体"),fill='purple'))

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
        try:
            if len(self.stoptext) > 0:
                for text in self.stoptext:
                    self.canvas.delete(text)
        except:
            print "5"
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
#             bottomnum = topnum
        else:
            topwidth = self.width / ((self.carnum + 1) / 2)
            bottomwidth = self.width / ((self.carnum - 1) / 2)
            topnum = (self.carnum + 1) / 2
#             bottomnum = (self.carnum - 1) / 2
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
        ttk.Label(self.tab2, text="网络拓扑").grid()
    
    def ShowData(self):
        
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
        clearbutton.grid(row=1,column=2,sticky = tk.W)
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

#         inputlb = tk.Listbox(sendgroup)
#         inputlb.grid(row=0, column=0,sticky=tk.N+tk.S+tk.E+tk.W)

    def NetSniffer(self):
        ttk.Label(self.tab4, text="网络抓包").grid()
    
    def SendData(self):
        print self.radiovalue.get()
    
    def pause(self):
        snifferthread = self.menu.uartform.snifferthread
        try:
            snifferthread.thread_stop = True
        except NameError:
            tkmes.showwarning("错误", "串口未启动!")
  
    def restart(self):
        snifferthread = self.menu.uartform.snifferthread
        try:
            snifferthread.thread_stop = False
            self.menu.uartform.snifferthread.openfile()
#             uartthread.openfile()
        except NameError:
            tkmes.showwarning("错误", "串口未启动!")
        
    def updatetext(self, chars):
        try:
            self.receivetext.insert(1.0, chars)
        except:
            print "6"
        
    def cleartext(self):
        self.receivetext.delete(1.0, tk.END)
    
    def opentxt(self):
        self.pause()
        self.menu.uartform.snifferthread.closefile()
        print self.menu.uartform.txtfilname
        os.startfile(self.menu.uartform.txtfilname)
#         try:
#             self.menu.uartform.snifferthread.clsoefile()
#             print self.menu.uartform.txtfilname
#             os.startfile(self.menu.uartform.txtfilname)
#         except:
#             pass
         
if __name__ == '__main__':
    global root
    root = MainRoot()
    root.rootmenu.show()
    root.ShowData()  # 数据显示
    root.NetStatus()  # 网络状态
    root.StopStatus()  # 停车状态
    root.NetSniffer()  # 网络抓包
    root.SysStatus()  # 状态栏

    root.mainloop()

    
