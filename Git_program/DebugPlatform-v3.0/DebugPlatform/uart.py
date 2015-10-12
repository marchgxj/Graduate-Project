# coding=utf-8
import _winreg as winreg
import itertools
import Tkinter as tk
import ttk
import serial
import time
import relaythread
import snifferthread
import threading
import identifythread
import tkMessageBox as tkmes

class GetSerialPorts(object):
    # list contains all port device info
    portList = []
    IterationError=''
    def enumerate_serial_ports(self):
        '''
        Parameter：

        Function：
            Uses the Win32 registry to return a iterator of serial 
            (COM) ports existing on this computer.
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        """ 
        """
        path = 'HARDWARE\\DEVICEMAP\\SERIALCOMM'
        try:
            key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, path)
        except WindowsError:
            tkmes.showwarning("错误！","没有找到串口！")
            return
            # raise self.IterationError
        self.portList=[]
        for i in itertools.count():
            try:
                val = winreg.EnumValue(key, i)
                self.portList.append(val)
            except EnvironmentError:
                break

        winreg.CloseKey(key)

    def port_list(self):
        '''
        Parameter：

        Function：
                            将串口号存在portlist中
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.portList=[]
        for x in range(len(self.portList)):
            tmp = self.portList[x]
            print tmp[0]
            print str(tmp[1])

class UartRoot(tk.Tk):
    """Container for all frames within the application"""
    
    errtext = '没有错'
    def __init__(self, comnum, frame, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)  
        self.stopbitcboxbuf='1bit'
        self.databitcboxbuf='8bit'
        self.parent = frame
        self.parent_menu = self.parent.rootmenu
        self.title("串口配置")
        self.geometry('250x400')
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
        self.bordratecbox = ttk.Combobox(self, width=10)
        self.bordratecbox['value'] = bordrate
        self.bordratecbox.set(self.parent_menu.bordratecboxbuf)
        self.bordratecbox.grid(row=1, column=1)
        
        ttk.Label(self, text="数据位:", padding=5).grid(row=2)
        databit = ['8bit', '7bit']
        self.databitcbox = ttk.Combobox(self, width=10)
        self.databitcbox['value'] = databit
        self.databitcbox.set(self.databitcboxbuf)
        self.databitcbox.grid(row=2, column=1)
        
        ttk.Label(self, text="停止位:", padding=5).grid(row=3)
        stopbit = ['1bit', '2bit']
        self.stopbitcbox = ttk.Combobox(self, width=10)
        self.stopbitcbox['value'] = stopbit
        self.stopbitcbox.set(self.stopbitcboxbuf)
        self.stopbitcbox.grid(row=3, column=1)
        
        ttk.Label(self, text="数据源类型:", padding=5).grid(row=4)
        self.datasourcecbox = ttk.Combobox(self, width=10)
        self.datasourcecbox['value'] = ("中继","抓包","识别")
        self.datasourcecbox.set(self.parent_menu.datasourcecboxbuf)
        self.datasourcecbox.grid(row=4, column=1)

        tk.Label(self, text="数据上传:").grid(row=5, column=0)
        self.updatamodecbox = ttk.Combobox(self,width=10)
        self.updatamodecbox.grid(row=5,column=1)
        self.updatamodecbox['value']=("开启","不开启")
        self.updatamodecbox.set(self.parent_menu.updatamodecboxbuf)

        self.uartopenbutton = ttk.Button(self, text='打开串口', command=self.Choseuart)
        self.uartopenbutton.grid(row=6, column=1)

        self.uartstatus = tk.Canvas(self, width=20, height=20, background='black')
        self.statusrec = self.uartstatus.create_rectangle(0, 0, 20, 20, fill='red')      
        self.uartstatus.grid(row=6, column=0)
        self.IsOpen(0)
        self.txtfilname = "sniffer-"+time.strftime('%Y-%m-%d_%H-%M-%S', time.localtime(time.time())) + '.txt'
        self.txtidentifyfilname = '..\Data\\'+time.strftime('%Y-%m-%d_%H-%M-%S', time.localtime(time.time())) + '.txt'
        self.buttonstatus = 0
        self.datamode = 0

    def showupdate(self):
        '''
        Parameter：

        Function：
                               串口 设置界面显示对应的串口号和波特率
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.bordratecbox.set(self.parent_menu.bordratecboxbuf)
        self.datasourcecbox.set(self.parent_menu.datasourcecboxbuf)
        self.updatamodecbox.set(self.parent_menu.updatamodecboxbuf)
        self.IsOpen(0)

    def Choseuart(self):
        if self.buttonstatus == 1:
            self.CloseUart()
        elif self.buttonstatus == 0:
            self.OpenUart()
        self.IsOpen()

    
    def OpenUart(self):
        '''
        Parameter：

        Function：
                               打开对应串口并创建相关线程
        Autor:xiaoxiami 2015.5.29
        Others：
        '''

#         mainform.root.status.set("%s",'sdf')

        try:
#             中继
            if self.datasourcecbox.current() == 0:
                self.datamode = self.updatamodecbox.current()
                self.parent_menu.updatamodecboxbuf = self.updatamodecbox.get()
                self.relaythread = relaythread.myThread(rootframe=self.parent,threadID=0, name='relay', port=self.comnumbox.get(), baud=self.bordratecbox.get())
                self.relaythread.setDaemon(True)
                self.relaythread.Createuart()
                self.relaythread.uart.open()
                self.relaythread.start()
#             sniffer   
            elif self.datasourcecbox.current() == 1:
                self.snifferthread = snifferthread.myThread(rootframe=self.parent,threadID=1, name='sniffer', port=self.comnumbox.get(), baud=self.bordratecbox.get(),filename = self.txtfilname)
                self.parent_menu.snifferthread = self.snifferthread
                self.snifferthread.setDaemon(True)
                self.snifferthread.start()
                # self.updatethread = threading.Thread(target=self.snifferthread.updatetext)
                # self.updatethread.setDaemon(True)
                # self.updatethread.start()
                # 识别
            elif self.datasourcecbox.current() == 2:
                self.identifythread = identifythread.myThread(rootframe=self.parent,threadID=1, name='identify',port=self.comnumbox.get(), baud=self.bordratecbox.get(),filename = self.txtidentifyfilname)
                self.identifythread.setDaemon(True)
                self.identifythread.Creatuart()
                self.identifythread.uart.open()
                self.identifythread.start()
            self.parent_menu.opened_uart.append(self.comnumbox.get())
            self.IsOpen(0)
            self.parent.status.setstatus('%s', self.comnumbox.get() + '已打开')
            self.parent_menu.bordratecboxbuf = self.bordratecbox.get()
            self.parent_menu.datasourcecboxbuf = self.datasourcecbox.get()
            self.parent_menu.datasourcecboxvalue = self.datasourcecbox.current()
            
        except serial.SerialException, error:
            self.errtext = str(error)
            text = tk.Label(self, text="串口被占用！")
            text.grid(row=7, column=1)
            print str(error)

    def CloseUart(self):
        '''
        Parameter：

        Function：
                               关闭串口
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        if self.comnumbox.get() in self.parent_menu.opened_uart:
            self.parent_menu.opened_uart.remove(self.comnumbox.get())
        self.IsOpen()
        if self.datasourcecbox.current() == 0:
            self.relaythread.uart.close()
            self.relaythread.killthread=True
        elif self.datasourcecbox.current() == 1:
            self.parent_menu.snifferthread.thread_stop = True
            self.parent_menu.snifferthread.uart.close()
        elif self.datasourcecbox.current() == 2:
            self.identifythread.uart.close()
            self.identifythread.thread_stop = True
        self.parent.status.setstatus('%s', self.comnumbox.get() + '已关闭')
        return
        try:
            if self.comnumbox.get() in self.parent_menu.opened_uart:
                self.parent_menu.opened_uart.remove(self.comnumbox.get())
            self.IsOpen(0)
            if self.datasourcecbox.current() == 0:
                self.relaythread.uart.close()
                self.relaythread.killthread=True
            elif self.datasourcecbox.current() == 1:
                self.parent_menu.snifferthread.thread_stop = True
                self.parent_menu.snifferthread.uart.close()
            elif self.datasourcecbox.current() == 2:
                self.identifythread.uart.close()
                self.identifythread.thread_stop = True
            self.parent.status.setstatus('%s', self.comnumbox.get() + '已关闭')
        except NameError:
            self.parent.status.setstatus('%s', '串口未打开')

    def IsOpen(self, *event):
        '''
        Parameter：
            event：回调事件
        Function：
                               更新提示图像
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        comnunm = self.comnumbox.get()
        if comnunm in self.parent_menu.opened_uart:
            self.uartstatus.itemconfig(self.statusrec, fill='green')
            self.uartopenbutton.configure(text = "关闭串口")
            self.buttonstatus = 1
        else:
            self.uartstatus.itemconfig(self.statusrec, fill='red')
            self.uartopenbutton.configure(text = "打开串口")
            self.buttonstatus = 0