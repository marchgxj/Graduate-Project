# coding=utf-8
import time
import urllib2
import urllib
import serial
import threading
from collections import deque
import platform

__author__ = 'Changxiaodong'


class Relay:
    def sendCommand(self, add, cmd):
        '''
        Parameter：add:address of node
                   cmd:command num

        Function：
                              向节点发送控制信息
        Autor:xiaoxiami 2015.12.26
        Others：控制信息可以由上位机输入，或从服务器上获取
        '''
        cmd_address = (chr(int(add)-20))
        cmd_cmd = (chr(int(cmd)))
        self.uart.write("\xAA")
        self.uart.write(cmd_address)
        self.uart.write(cmd_cmd)
        self.uart.write("\xBB")
        ack = self.uart.read(1)
        if (ack != ""):
            if (ord(ack) == 0xAA):  # 收ack
                return 1
            else:
                return 0

    def sendCommandfromServer(self, feedback):
        '''
        Parameter：

        Function：
                              从服务器获取控制命令，发送给中继
        Autor:xiaoxiami 2015.12.26
        Others：
        '''
        for key, value in feedback.items():
            if key != "Status":
                add = int(str(key).split("-")[-1])
                for i in range(10):
                    if self.sendCommand(add, value):
                        break

    def run(self):
        '''
        Parameter：

        Function：
                从串口接收到数据后上传至服务器
        Autor:xiaoxiami 2015.5.29
        Others：时间长会出错。可以修改，当数据与当前界面显示数据发生变化时在更新界面
        '''
        self.uart = serial.Serial()
        if platform.system() == "Linux":
            self.port = "/dev/ttyAMA0"
            filepath = "./Log/" + time.strftime('%Y-%m-%d_%H-%M-%S', time.localtime(time.time())) + '.txt'
        else:
            self.port = "COM3"
            filepath = ".\Log\\" + time.strftime('%Y-%m-%d_%H-%M-%S', time.localtime(time.time())) + '.txt'
        self.uart.port = self.port
        self.uart.baudrate = 115200
        self.uart.open()


        self.file = open(filepath, "a+")
        self.file.write("Program Start at:" + time.strftime('%Y-%m-%d  %H:%M:%S',time.localtime(time.time())) + "\n")
        self.file.close()
        print "running"

        self.notedata = []
        count = 0
        data = {}
        debug_count = 0
        data["relay_id"] = "0086-110108-00022105-01"
        data["park_id"] = "22105"
        data["data"] = ""
        self.upload_databuf = deque()
        self.netUploadThread = threading.Thread(target=self.dataUpload)
        self.netUploadThread.setDaemon(True)
        #self.netUploadThread.start()

        while (1):
            try:
                ordbuf = self.uart.read(1)
            except serial.SerialException:
                pass
            if (ordbuf != ""):
                if ord(ordbuf) == 0x7D:
                    try:
                        ordbuf = ord(self.uart.read(1))
                    except serial.SerialException:
                        pass

                    if ordbuf == 0x7E:
                        count += 1
                        try:
                            length = ord(self.uart.read(1))  # 读出这一包要发的节点个数
                        except serial.SerialException:
                            pass
                        print length
                        self.notedata = []
                        data["data"] = ""
                        for i in range(length):
                            try:
                                num = ord(self.uart.read(1)) << 8 | ord(self.uart.read(1))
                                status = ord(self.uart.read(1))
                                num += 20
                            except serial.SerialException:
                                pass
                            self.notedata.append(num)  # 偶数位为地址
                            self.notedata.append(status)  # 奇数位为数据
                            data["data"] = data["data"] + "0086-110108-00022105-" + str(num).zfill(4) + "|"\
                                           + str(status) + ','

                        if len(self.notedata) == length * 2:
                            self.uart.write("o")
                        else:
                            print "uart data error"
                            try:
                                self.uart.read(self.uart.inWaiting())  # 清空串口缓冲区内容
                            except serial.SerialException:
                                pass
                        uploaddatacut = data["data"]
                        data["data"] = uploaddatacut[:-1]
                        print data["data"]
                        #self.upload_databuf.append(urllib.urlencode(data))
                        post_data = urllib.urlencode(data)
                        data["data"] = ""
                        try:
                            '''上传全部数据'''
                            response = urllib2.urlopen("http://www.xiaoxiami.space/info/post/", post_data, timeout=1)
                            feedback = eval(response.read())
                            print feedback
                            self.sendCommandfromServer(feedback)
                        except:
                            pass

                    elif ordbuf == 0x7F:
                        debug_count += 1
                        err_msg = self.uart.readline()
                        err_msg = err_msg[:-1]
                        self.file = open(filepath, "a+")
                        self.file.write(time.strftime('%Y-%m-%d  %H:%M:%S',
                                                      time.localtime(time.time())) + ":  " + err_msg + "," + str(
                            debug_count) + "\n")

    def dataUpload(self):
        '''
        Parameter：

        Function：
                               将数据上传至服务器
        Autor:xiaoxiami 2015.12.3
        Others：
        '''
        while True:
            if len(self.upload_databuf):
                post_data = self.upload_databuf.popleft()
                print post_data
                start = time.clock()
                try:
                    '''上传全部数据'''
                    response = urllib2.urlopen("http://123.57.37.66:8080/sensor/post/status", post_data, timeout=5)
                    end = time.clock()
                    print "time:" + str(end - start)
                    serverresponse = response.read()
                    serverresponsedic = eval(serverresponse)
                    print serverresponsedic["err_msg"]
                except:
                    pass


if __name__ == '__main__':
    relay = Relay()
    relay.run()
