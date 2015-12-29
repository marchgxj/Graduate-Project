# coding=utf-8
import threading
import serial
import time
import os


class myThread(threading.Thread):
    def __init__(self, threadID, name, port, baud, rootframe, filename):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.root = rootframe
        self.app = self.root.appFrame
        self.statusbar = rootframe
        self.currenttab = 0
        self.thread_stop = True
        self.magneticdata = 0
        self.carstatus = 0
        self.port = port
        self.baud = baud
        self.VALUE_LENGTH = 46
        self.VALUE_NUM = 26
        self.value = [0 for n in range(self.VALUE_NUM)]

        self.readenable = 0
        if not os.path.exists("../Data/Identify"):
            os.makedirs('../Data/Identify')
        self.filename = filename
        # with open(filename, 'w') as file:
        #     file.write("Identify Start at:" + time.strftime('%Y-%m-%d  %H:%M:%S',time.localtime(time.time())) + "\n")

        self.sensordata = []
        self.framecount = 0
        self.update = threading.Thread(target=self.update)
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
        self.framecount = 0
        self.app.identifyuartopen = 1
        while (1):
            if (self.thread_stop == True and self.uart.isOpen() == True):
                try:
                    self.uart.read(self.uart.inWaiting())
                except serial.SerialException:
                    self.statusbar.status.setdata("串口被拔出,插入后请重新打开")
                    break
                time.sleep(0.5)
            while (self.thread_stop == False and self.uart.isOpen() == True):
                self.currenttab = self.app.tab
                if self.currenttab == 3:
                    try:
                        buf = self.uart.read(1)
                    except serial.SerialException:
                        self.statusbar.status.setdata("串口被拔出,插入后请重新打开")
                        self.thread_stop = True
                        break
                    if len(buf) != 0:
                        if ord(buf) == 0x7D:
                            buf = []
                            # 加变量需要修改
                            try:
                                buf += self.uart.read(self.VALUE_LENGTH)
                            except serial.SerialException:
                                self.statusbar.status.setdata("串口被拔出,插入后请重新打开")
                                self.thread_stop = True
                                break

                            self.file = open(self.filename, "a+")
                            self.file.write("|" + str(time.time()) + "|")

                            # 加变量需要修改
                            self.value[0] = (ord(buf[0]) << 8 | ord(buf[1]))
                            self.value[1] = (ord(buf[2]) << 8 | ord(buf[3]))
                            self.value[2] = (ord(buf[4]) << 8 | ord(buf[5]))
                            self.value[3] = (ord(buf[6]) << 8 | ord(buf[7]))
                            self.value[4] = (ord(buf[8]) << 8 | ord(buf[9]))
                            self.value[5] = (ord(buf[10]))
                            self.value[6] = (ord(buf[11]))
                            self.value[7] = (ord(buf[12]) << 8 | ord(buf[13]))
                            self.value[8] = (ord(buf[14]) << 8 | ord(buf[15]))
                            self.value[9] = (ord(buf[16]) << 8 | ord(buf[17]))
                            self.value[10] = (ord(buf[18]) << 8 | ord(buf[19]))
                            self.value[11] = (ord(buf[20]))
                            self.value[12] = (ord(buf[21]))
                            self.value[13] = (ord(buf[22]) << 8 | ord(buf[23]))
                            self.value[14] = (ord(buf[24]) << 8 | ord(buf[25]))
                            self.value[15] = (ord(buf[26]) << 8 | ord(buf[27]))
                            self.value[16] = (ord(buf[28]) << 8 | ord(buf[29]))
                            self.value[17] = (ord(buf[30]) << 8 | ord(buf[31]))
                            self.value[18] = (ord(buf[32]) << 8 | ord(buf[33]))
                            self.value[19] = (ord(buf[34]) << 8 | ord(buf[35]))
                            self.value[20] = (ord(buf[36]) << 8 | ord(buf[37]))
                            self.value[21] = (ord(buf[38]) << 8 | ord(buf[39]))
                            self.value[22] = (ord(buf[40]) << 8 | ord(buf[41]))
                            self.value[23] = (ord(buf[42]) << 8 | ord(buf[43]))
                            self.value[24] = (ord(buf[44]))
                            self.value[25] = (ord(buf[45]))

                            for i in range(self.VALUE_NUM):
                                self.file.write(str(self.value[i]) + " ")
                            self.file.write("\n")

                            self.file.close()
                            try:
                                self.uart.read(self.uart.inWaiting())
                            except serial.SerialException:
                                self.statusbar.status.setdata("串口被拔出,插入后请重新打开")
                                self.thread_stop = True
                                break
                            self.framecount += 1

                        #                     time.sleep(0.5)

    def update(self):
        while (1):
            if (self.sensordata != '' and self.count != 0):
                self.app.Drawonce(count=self.count, value=self.sensordata)
                self.root.status.setdata('RX:%s ', self.sensordata)
                time.sleep(0.2)

    def Creatuart(self):
        self.uart = serial.Serial()
        self.uart.port = self.port
        self.uart.baudrate = self.baud


