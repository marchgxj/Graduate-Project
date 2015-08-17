# coding=utf-8
import threading
import serial
import time
import binascii
class myThread (threading.Thread):   
    def __init__(self, threadID, name, port, baud,rootframe,filename):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.root = rootframe
        self.app = self.root.appFrame
        self.currenttab = 0
        self.thread_stop = True
        self.magneticdata = 0
        self.carstatus = 0
        self.port = port
        self.baud = baud


        self.filename = filename
        self.sensordata=[]
        self.count = 0
        self.update = threading.Thread(target = self.update)
        self.update.setDaemon(True)
#         self.update.start()
   
    def run(self):
        '''
        Parameter：
           
        Function：
                               接收串口数据，拆包，显示
        Autor:xiaoxiami 2015.5.29
        Others：线程   
                                    当前数据格式：帧头 0x7D 5个16位传感器数值 高位在前  1个8位停车状态  1个16位斜率 高位在前 有符号型  帧尾0x7E    
        '''
        self.count = 0
        self.app.identifyuartopen = 1
        while(1):
            if(self.thread_stop == True and self.uart.isOpen()==True):
                self.uart.read(self.uart.inWaiting())
                time.sleep(0.5)
            while(self.thread_stop == False and self.uart.isOpen()==True):
                self.currenttab = self.app.tab
                if self.currenttab == 3:
                    buf = self.uart.read(1)
                    if len(buf)!= 0:
                        if ord(buf)==0x7D:
                            # 不是采集5位需要改的
                            buf+=self.uart.read(4)
                            for v in buf:
                                self.file = open(self.filename,"a+")
                                self.file.write(binascii.b2a_hex(v)+" ")
                                self.file.close()
                            # 不是采集5位需要改的   
                            self.valuex=[]
                            self.valuex.append(ord(buf[1])<<8|ord(buf[2]))
                            self.valuey=[]
                            self.valuey.append(ord(buf[3])<<8|ord(buf[4]))
                            # self.sensordata.append(ord(buf[3])<<8|ord(buf[4]))
                            # self.sensordata.append(ord(buf[5])<<8|ord(buf[6]))
                            # self.sensordata.append(ord(buf[7])<<8|ord(buf[8]))
                            # self.sensordata.append(ord(buf[9])<<8|ord(buf[10]))
                            self.app.Drawonce(count=self.count,valuex=self.valuex,valuey=self.valuey)
                            try:
                                self.root.status.setdata('X:%s Y:%s',self.valuex,self.valuey)
                            except:
                                print "I'm identifythread line 66"
                            self.uart.read(self.uart.inWaiting())
                            self.count+=1
#                     time.sleep(0.5)
                    #每次有5个新数据
                    # 不是采集5位需要改的

    def update(self):
        while(1):
            if(self.sensordata != '' and self.count!=0):
                self.app.Drawonce(count=self.count,value=self.sensordata)
                self.root.status.setdata('RX:%s ',self.sensordata) 
                time.sleep(0.2)

    def Creatuart(self):
        self.uart= serial.Serial()
        self.uart.port = self.port
        self.uart.baudrate = self.baud