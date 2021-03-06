# coding=utf-8
import Tkinter as tk
import ttk

class CarStopRoot(tk.Tk):
    def __init__(self,rootframe, *args, **kwargs):
        self.locat = 0
        self.carnum = 0
        self.datamode = 0
        self.updatamode = 1
        self.root = rootframe
        self.menu = self.root.rootmenu
        self.app = rootframe.appFrame
        
    def show(self, *args, **kwargs):
        '''
        Parameter：
           
        Function：
                               停车设置显示
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        tk.Tk.__init__(self, *args, **kwargs)
        self.locat = ""
        self.title("停车设置")
        tmpcnf = '%dx%d+%d+%d' % (100, 100, 0, 0)
        self.geometry(tmpcnf)
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
        self.carnumspinbox = tk.Spinbox(self, from_=16, to=50, width=10)

        self.carnumspinbox.grid(row=0, column=1)
        carlolabel = tk.Label(self, text="车位位置:")
        carlolabel.grid(row=2, column=0,sticky=tk.W)
        self.carlocatecbox = ttk.Combobox(self, width=10)
        self.carlocatecbox['value'] = ("方兴大厦", "其他")
        self.carlocatecbox.set(self.menu.carlocatecboxbuf)
        self.carlocatecbox.grid(row=2, column=1,sticky=tk.W)
        
        # tk.Label(self, text="数据上传:").grid(row=3, column=0)
        # self.updatamodecbox = ttk.Combobox(self,width=10)
        # self.updatamodecbox.grid(row=3,column=1)
        # self.updatamodecbox['value']=("开启","不开启")
        # self.updatamodecbox.set(self.menu.updatamodecboxbuf)
        
        tk.Label(self, text="数据源:").grid(row=4, column=0)
        self.datamodecbox = ttk.Combobox(self,width=10)
        self.datamodecbox.grid(row=4,column=1)
        self.datamodecbox['value']=("串口数据","网络数据")
        self.datamodecbox.set(self.menu.datamodecboxbuf)
        ttk.Button(self, text="确认", command=self.carstopconfirm).grid(row=5, column=0, columnspan=2)

    def showupdata(self):
        '''
        Parameter：
           
        Function：
                               更新停车设置对应下拉菜单的值
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.carlocatecbox.set(self.menu.carlocatecboxbuf)
        # self.updatamodecbox.set(self.menu.updatamodecboxbuf)
        self.datamodecbox.set(self.menu.datamodecboxbuf)
  
    def carstopconfirm(self):
        '''
        Parameter：
           
        Function：
                               将对应的设定值传递给对应参数
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.locat = self.carlocatecbox.current()
        self.carnum = int(self.carnumspinbox.get())
        self.datamode = self.datamodecbox.current()
        # self.updatamode = self.updatamodecbox.current()
        
        self.menu.carlocatecboxbuf = self.carlocatecbox.get()
        self.menu.datamodecboxbuf = self.datamodecbox.get()
        # self.menu.updatamodecboxbuf = self.updatamodecbox.get()
        global root
        self.app.DrawParking()
        # try:
        #     self.exit()
        # except:
        #     pass

