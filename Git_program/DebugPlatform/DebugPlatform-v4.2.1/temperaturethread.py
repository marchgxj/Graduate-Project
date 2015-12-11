# coding=utf-8
import threading
import serial
import time
import os
import platform


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
        self.filepath, self.file = self.createFileandPath()
        self.value_buf = [[0 for col in range(4)] for row in range(4)]  # x值，y值，z值,节点物理地址
        self.value_buf_infile = [[0 for col in range(4)] for row in range(12)]
        self.sensordata = []
        self.update = threading.Thread(target=self.update)
        self.update.setDaemon(True)
        self.started = False

        #self.update.start()

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
        error = 0
        while True:
            while self.uart.isOpen():
                self.currenttab = self.app.tab
                if self.currenttab == 6 and self.started:
                    error = 0
                    buf = self.uart.read(1)
                    if len(buf) != 0:
                        if ord(buf) == 0x7D:
                            buf = []
                            # 加变量需要修改
                            buf += self.uart.read(6)

                            nodeaddress = (ord(buf[0]) << 8 | ord(buf[1]))
                            acix_type = ord(buf[2])
                            value = (ord(buf[3]) << 8 | ord(buf[4]))
                            if nodeaddress < 13 and acix_type < 4 and value < 4096:
                                self.sensordata = [nodeaddress, acix_type, value]
                                if added_node_num < 4 and not address_dic.has_key(nodeaddress):
                                    address_dic[nodeaddress] = added_node_num
                                    added_node_num += 1
                                if nodeaddress in range(1, 13):
                                    self.value_buf_infile[nodeaddress - 1][acix_type] = value
                                    try:
                                        nodenum = address_dic[nodeaddress]
                                    except KeyError:
                                        pass

                                    if nodenum in range(4) and acix_type in range(3) and address_dic.has_key(nodeaddress):
                                        self.value_buf[nodenum][acix_type] = value
                                        self.value_buf[nodenum][3] = nodeaddress
                                    if acix_type == 2:
                                        self.file[nodeaddress - 1] = open(self.filepath[nodeaddress - 1], "a+")
                                        timenow = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time()))
                                        self.file[nodeaddress - 1].write(
                                            timenow + "," + str(self.value_buf_infile[nodeaddress - 1][0]) + "," +
                                            str(self.value_buf_infile[nodeaddress - 1][1]) + "," +
                                            str(self.value_buf_infile[nodeaddress - 1][2]) + "\n")
                                        self.file[nodeaddress - 1].close()
                            self.uart.read(self.uart.inWaiting())

    def update(self):
        while True:
            if self.sensordata:
                self.root.status.setdata('RX:%s ', self.sensordata)
                time.sleep(0.2)

    def createFileandPath(self, temp="0"):
        '''
        Parameter：

        Function：
                               根据系统类型创建文件并且返回文件路径
        Autor:xiaoxiami 2015.12.1
        Others：
        input: temp:温度
        return:filepath[]:4个文件路径列表
               file[]:4个打开文件指针
        '''
        timenow = time.strftime('%Y-%m-%d_%H-%M-%S', time.localtime(time.time())) + '_'
        if platform.system() == "Linux":
            os.makedirs(r'../Data/Temperature/' + timenow + temp)
            filepath = ['../Data/Temperature/' + timenow + temp + '/Node_1' + '.txt',
                        '../Data/Temperature/' + timenow + temp + '/Node_2' + '.txt',
                        '../Data/Temperature/' + timenow + temp + '/Node_3' + '.txt',
                        '../Data/Temperature/' + timenow + temp + '/Node_4' + '.txt',
                        '../Data/Temperature/' + timenow + temp + '/Node_5' + '.txt',
                        '../Data/Temperature/' + timenow + temp + '/Node_6' + '.txt',
                        '../Data/Temperature/' + timenow + temp + '/Node_7' + '.txt',
                        '../Data/Temperature/' + timenow + temp + '/Node_8' + '.txt',
                        '../Data/Temperature/' + timenow + temp + '/Node_9' + '.txt',
                        '../Data/Temperature/' + timenow + temp + '/Node_10' + '.txt',
                        '../Data/Temperature/' + timenow + temp + '/Node_11' + '.txt',
                        '../Data/Temperature/' + timenow + temp + '/Node_12' + '.txt']
            file1 = open('../Data/Temperature/' + timenow + temp + '/Node_1' + '.txt', "a+")
            file2 = open('../Data/Temperature/' + timenow + temp + '/Node_2' + '.txt', "a+")
            file3 = open('../Data/Temperature/' + timenow + temp + '/Node_3' + '.txt', "a+")
            file4 = open('../Data/Temperature/' + timenow + temp + '/Node_4' + '.txt', "a+")
            file5 = open('../Data/Temperature/' + timenow + temp + '/Node_5' + '.txt', "a+")
            file6 = open('../Data/Temperature/' + timenow + temp + '/Node_6' + '.txt', "a+")
            file7 = open('../Data/Temperature/' + timenow + temp + '/Node_7' + '.txt', "a+")
            file8 = open('../Data/Temperature/' + timenow + temp + '/Node_8' + '.txt', "a+")
            file9 = open('../Data/Temperature/' + timenow + temp + '/Node_9' + '.txt', "a+")
            file10 = open('../Data/Temperature/' + timenow + temp + '/Node_10' + '.txt', "a+")
            file11 = open('../Data/Temperature/' + timenow + temp + '/Node_11' + '.txt', "a+")
            file12 = open('../Data/Temperature/' + timenow + temp + '/Node_12' + '.txt', "a+")
        else:
            os.makedirs(r'..\Data\Temperature\\' + timenow + temp)
            filepath = ['..\Data\Temperature\\' + timenow + temp + '\Node_1' + '.txt',
                        '..\Data\Temperature\\' + timenow + temp + '\Node_2' + '.txt',
                        '..\Data\Temperature\\' + timenow + temp + '\Node_3' + '.txt',
                        '..\Data\Temperature\\' + timenow + temp + '\Node_4' + '.txt',
                        '..\Data\Temperature\\' + timenow + temp + '\Node_5' + '.txt',
                        '..\Data\Temperature\\' + timenow + temp + '\Node_6' + '.txt',
                        '..\Data\Temperature\\' + timenow + temp + '\Node_7' + '.txt',
                        '..\Data\Temperature\\' + timenow + temp + '\Node_8' + '.txt',
                        '..\Data\Temperature\\' + timenow + temp + '\Node_9' + '.txt',
                        '..\Data\Temperature\\' + timenow + temp + '\Node_10' + '.txt',
                        '..\Data\Temperature\\' + timenow + temp + '\Node_11' + '.txt',
                        '..\Data\Temperature\\' + timenow + temp + '\Node_12' + '.txt']
            file1 = open('..\Data\Temperature\\' + timenow + temp + '\Node_1' + '.txt', "a+")
            file2 = open('..\Data\Temperature\\' + timenow + temp + '\Node_2' + '.txt', "a+")
            file3 = open('..\Data\Temperature\\' + timenow + temp + '\Node_3' + '.txt', "a+")
            file4 = open('..\Data\Temperature\\' + timenow + temp + '\Node_4' + '.txt', "a+")
            file5 = open('..\Data\Temperature\\' + timenow + temp + '\Node_5' + '.txt', "a+")
            file6 = open('..\Data\Temperature\\' + timenow + temp + '\Node_6' + '.txt', "a+")
            file7 = open('..\Data\Temperature\\' + timenow + temp + '\Node_7' + '.txt', "a+")
            file8 = open('..\Data\Temperature\\' + timenow + temp + '\Node_8' + '.txt', "a+")
            file9 = open('..\Data\Temperature\\' + timenow + temp + '\Node_9' + '.txt', "a+")
            file10 = open('..\Data\Temperature\\' + timenow + temp + '\Node_10' + '.txt', "a+")
            file11 = open('..\Data\Temperature\\' + timenow + temp + '\Node_11' + '.txt', "a+")
            file12 = open('..\Data\Temperature\\' + timenow + temp + '\Node_12' + '.txt', "a+")
        file = []
        file.append(file1)
        file.append(file2)
        file.append(file3)
        file.append(file4)
        file.append(file5)
        file.append(file6)
        file.append(file7)
        file.append(file8)
        file.append(file9)
        file.append(file10)
        file.append(file11)
        file.append(file12)
        return filepath, file

    def Creatuart(self):
        self.uart = serial.Serial()
        self.uart.port = self.port
        self.uart.baudrate = self.baud

    def dataToExcel(self):
        pass
