# coding=utf-8

import threading
import serial
import time
__author__ = 'Changxiaodong'

class myThread(threading.Thread):
    def __init__(self, threadID, name, port, baud, rootframe, filename):
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
        self.value = []
        for i in range(26):
            self.value.append(0)
        self.readenable = 0

        self.filename = filename
        self.sensordata = []
        self.framecount = 0

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
        self.framecount = 0
        self.app.identifyuartopen = 1
        while (1):
            self.currenttab = self.app.tab
            if self.currenttab == 5:
                buf = self.uart.read(1)

                if len(buf) != 0:
                        if ord(buf) == 0x7E:
                            buf=[]
                            buf += self.uart.read(20)
                            self.value[0] = (ord(buf[14]) << 8 | ord(buf[15]))
                            self.value[1] = (ord(buf[16]) << 8 | ord(buf[17]))
                            self.value[3] = (ord(buf[12]) << 8 | ord(buf[13]))
                            if(float(self.value[0]!=0)):
                                self.value[2] = 100-float(self.value[3])/float(self.value[0])*100
                                self.value[2] = "%.2f" % self.value[2]
                            self.root.status.setdata('RX:%s ', self.value)



    def Creatuart(self):
        self.uart = serial.Serial()
        self.uart.port = self.port
        self.uart.baudrate = self.baud