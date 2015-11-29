# coding=utf-8
import threading
import serial
import time
import os


class myThread(threading.Thread):
    def __init__(self, threadID, name, port, baud, rootframe):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.root = rootframe
        self.app = self.root.appFrame
        self.currenttab = 0
        self.thread_stop = True

        self.port = port
        self.baud = baud
        timenow = time.strftime('%Y-%m-%d_%H-%M-%S', time.localtime(time.time()))
        os.makedirs(r'..\Data\\' + timenow)
        self.filepath = ['..\Data\\' + timenow + '\Node 1.txt',
                    '..\Data\\' + timenow + '\Node 2.txt',
                    '..\Data\\' + timenow + '\Node 3.txt',
                    '..\Data\\' + timenow + '\Node 4.txt']
        file1 = open('..\Data\\' + timenow + '\Node 1.txt', "a+")
        file2 = open('..\Data\\' + timenow + '\Node 2.txt', "a+")
        file3 = open('..\Data\\' + timenow + '\Node 3.txt', "a+")
        file4 = open('..\Data\\' + timenow + '\Node 4.txt', "a+")
        self.file = []
        self.file.append(file1)
        self.file.append(file2)
        self.file.append(file3)
        self.file.append(file4)

        self.value_buf = [[0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0]]  # x值，y值，z值,节点物理地址

        self.update = threading.Thread(target=self.update)
        self.update.setDaemon(True)

    #         self.update.start()

    def run(self):
        '''
        Parameter：

        Function：
                               接收串口数据，拆包，显示
        Autor:xiaoxiami 2015.11.27
        Others：线程
                                    当前数据格式：
                                    [1 bytes]帧头0x7D,
                                    [2 bytes]节点物理地址,
                                    [1 bytes]轴类型：x->1|y->2|z->3,
                                    [1bytes]数据高8，
                                    [1bytes]数据低8,
                                    [1bytes]帧尾0x7E

        '''
        self.app.temperatureuartopen = 1
        address_dic = {}
        added_node_num = 0
        while True:
            while self.uart.isOpen():
                self.currenttab = self.app.tab
                if self.currenttab == 6:
                    buf = self.uart.read(1)
                    if len(buf) != 0:
                        if ord(buf) == 0x7D:
                            buf = []
                            # 加变量需要修改
                            buf += self.uart.read(6)

                            nodeaddress = (ord(buf[0]) << 8 | ord(buf[1]))
                            acix_type = ord(buf[2])
                            value = (ord(buf[3]) << 8 | ord(buf[4]))

                            if not address_dic.has_key(nodeaddress):
                                if added_node_num < 4:
                                    address_dic[nodeaddress] = added_node_num
                                    added_node_num += 1
                                else:
                                    print "temperaturethread line 78"

                            try:
                                nodenum = address_dic[nodeaddress]
                            except KeyError:
                                nodenum = 5
                                print 'temperaturethread line 82'

                            if nodenum in range(4) and acix_type in range(3):
                                self.value_buf[nodenum][acix_type] = value
                                self.value_buf[nodenum][3] = nodeaddress
                            if acix_type == 2:
                                self.file[nodenum] = open(self.filepath[nodenum],"a+")
                                timenow = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time()))
                                self.file[nodenum].write(timenow + "|" + str(self.value_buf[nodenum][0]) + "," +
                                                         str(self.value_buf[nodenum][1]) + "," +
                                                         str(self.value_buf[nodenum][2]) + "\n")
                                self.file[nodenum].close()
                            self.uart.read(self.uart.inWaiting())

    def update(self):
        while True:
            if self.sensordata != '' and self.count != 0:
                self.app.Drawonce(count=self.count, value=self.sensordata)
                self.root.status.setdata('RX:%s ', self.sensordata)
                time.sleep(0.2)

    def Creatuart(self):
        self.uart = serial.Serial()
        self.uart.port = self.port
        self.uart.baudrate = self.baud
