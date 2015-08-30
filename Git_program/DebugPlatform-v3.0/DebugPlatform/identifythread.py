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
        self.value=[]


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
                            buf=[]
                            # 加变量需要修改
                            buf+=self.uart.read(16)
                            self.file = open(self.filename,"a+")
                            self.file.write("|"+str(time.time())+"|")
                            # for v in buf:
                                # self.file.write(time.time())
                                # self.file.write(binascii.b2a_hex(v)+" ")

                            # 加变量需要修改

                            self.value=[]
                            self.value.append(ord(buf[0])<<8|ord(buf[1]))
                            self.value.append(ord(buf[2])<<8|ord(buf[3]))
                            self.value.append(ord(buf[4])<<8|ord(buf[5]))
                            self.value.append(ord(buf[6])<<8|ord(buf[7]))
                            self.value.append(ord(buf[8])<<8|ord(buf[9]))
                            self.value.append(ord(buf[10]))
                            self.value.append(ord(buf[11]))
                            self.value.append(ord(buf[12])<<8|ord(buf[13]))
                            self.value.append(ord(buf[14])<<8|ord(buf[15]))

                            #加变量需要修改
                            self.file.write(str(self.value[0])+" ")
                            self.file.write(str(self.value[1])+" ")
                            self.file.write(str(self.value[2])+" ")
                            self.file.write(str(self.value[3])+" ")
                            self.file.write(str(self.value[4])+" ")
                            self.file.write(str(self.value[5])+" ")
                            self.file.write(str(self.value[6])+" ")
                            self.file.write(str(self.value[7])+" ")
                            self.file.write(str(self.value[8])+" ")
                            self.file.close()
                            self.uart.read(self.uart.inWaiting())
                            self.count+=1
#                     time.sleep(0.5)


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