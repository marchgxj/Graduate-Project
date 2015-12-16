# coding=utf-8
import time
import threading
import urllib2
import urllib
import serial
import os
from collections import deque


class myThread(threading.Thread):
    def __init__(self, threadID, name, port, baud, rootframe):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.thread_stop = False
        self.port = port
        self.baud = baud
        # self.uart= serial.Serial(port = self.port,baudrate = self.baud)
        self.statusbar = rootframe
        self.showdata = rootframe
        self.stopcar = rootframe
        self.root = rootframe
        self.menu = rootframe.rootmenu
        self.app = self.root.appFrame
        self.carstoproot = self.menu.carstoproot
        self.uartroot = self.menu.uartform
        self.radiovalue = self.app.radiovalue.get()
        self.data = []
        self.threadstartflag = 0
        self.datatoshow = ''
        self.content = {}
        self.timeoutflag = 0

        self.killthread = False
        self.thread_stop = False
        self.notedata = []
        self.longdata = [1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0]
        self.longdatastr = ""
        self.cmdaddress = ""
        self.Control_Resend = 0
        if not os.path.exists("../Log"):
            os.makedirs('../Log')
        self.filename = "../Log/" + time.strftime('%Y-%m-%d_%H-%M-%S', time.localtime(time.time())) + '.txt'
        self.logfile = open(self.filename, "a+")
        self.logfile.write("Program Start at:" + time.strftime('%Y-%m-%d  %H:%M:%S',time.localtime(time.time())) + "\n")
        self.logfile.close()
        self.netdatabuf = ''
        self.upload_databuf = deque()
        self.netuploadthread = threading.Thread(target=self.dataUpload)
        self.netuploadthread.setDaemon(True)


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
                start = time.clock()
                try:
                    '''上传全部数据'''
                    response = urllib2.urlopen("http://123.57.37.66:8080/sensor/post/status", post_data, timeout=5)
                    serverresponse = response.read()
                    serverresponsedic = eval(serverresponse)
                    end = time.clock()
                    self.statusbar.status.setstatus('网络延时:%s' + "  " + serverresponsedic["err_msg"],
                                                    str(end - start))
                except:
                    self.statusbar.status.setstatus('%s', "网络连接超时，请检查网络或关闭数据上传下载功能")

    def run(self):
        '''
        Parameter：
           
        Function：
                              从串口数据中获取停车信息，并且更新界面
        Autor:xiaoxiami 2015.5.29
        Others：时间长会出错。可以修改，当数据与当前界面显示数据发生变化时在更新界面
        '''
        a = 0
        count = 0
        debug_count = 0
        data = {}
        data["relay_id"] = "0086-110108-00022105-01"
        data["park_id"] = "22105"
        data["data"] = ""
        uploaddatacut = ""
        self.datatoshow = ''
        while (1):
            if self.showdata.appFrame.tab == 0 or self.showdata.appFrame.tab == 4:
                try:
                    ordbuf = self.uart.read(1)
                except:
                    self.statusbar.status.setdata("串口被拔出,插入后请重新打开")
                    break
                if ordbuf != "":
                    if ord(ordbuf) == 0x7D:
                        try:
                            ordbuf = ord(self.uart.read(1))
                        except serial.SerialException:
                            self.statusbar.status.setdata("串口被拔出,插入后请重新打开")
                            break
                        if ordbuf == 0x7E:
                            count += 1
                            try:
                                length = ord(self.uart.read(1))  # 读出这一包要发的节点个数
                            except serial.SerialException:
                                self.statusbar.status.setdata("串口被拔出,插入后请重新打开")
                                break
                            self.notedata = []
                            data["data"] = ""
                            for i in range(length):
                                try:
                                    try:
                                        num = ord(self.uart.read(1)) << 8 | ord(self.uart.read(1))
                                        status = ord(self.uart.read(1))
                                    except serial.SerialException:
                                        self.statusbar.status.setdata("串口被拔出,插入后请重新打开")
                                        break

                                    self.notedata.append(num)  # 偶数位为地址
                                    self.notedata.append(status)  # 奇数位为数据
                                    data["data"] = data["data"] + "0086-110108-00022105-" + str(num).zfill(4) + "|"\
                                           + str(status) + ','
                                    self.datatoshow = self.datatoshow + str(num).zfill(4) + "|"
                                    self.datatoshow = self.datatoshow + str(status) + ","
                                except:
                                    print "self.uart.read length 0 string"
                            if len(self.notedata) == length * 2:
                                self.uart.write("o")
                            else:
                                print "uart data error"
                                try:
                                    self.uart.read(self.uart.inWaiting())  # 清空串口缓冲区内容
                                except serial.SerialException:
                                        self.statusbar.status.setdata("串口被拔出,插入后请重新打开")
                                        break

                            uploaddatacut = data["data"]
                            data["data"] = uploaddatacut[:-1]

                            if self.showdata.appFrame.tab == 0:
                                self.statusbar.status.setdata('串口数据:%s 计数:%s', data["data"], count)
                            if self.stopcar.appFrame.carnum == 0:
                                self.statusbar.status.setstatus('%s', "未配置停车个数")
                            else:
                                self.stopcar.appFrame.stopcaronce(self.datatoshow[:-1])
                            self.datatoshow = ''
                            post_data = urllib.urlencode(data)
                            data["data"] = ""
                            start = time.clock()
                            try:
                                '''上传全部数据'''
                                response = urllib2.urlopen("http://123.57.37.66:8080/sensor/post/status", post_data, timeout=1)
                                serverresponse = response.read()
                                serverresponsedic = eval(serverresponse)
                                end = time.clock()
                                self.statusbar.status.setstatus('网络延时:%s' + "  " + serverresponsedic["err_msg"],
                                                                str(end - start))
                            except:
                                self.statusbar.status.setstatus('%s', "网络连接超时，请检查网络或关闭数据上传下载功能")
                    elif (ordbuf) == 0x7F:
                        debug_count += 1
                        err_msg = self.uart.readline()
                        err_msg = err_msg[:-1]
                        self.statusbar.status.setdata('Debug Msg:  %s   %s', err_msg, debug_count)
                        self.logfile = open(self.filename, "a+")
                        self.logfile.write(time.strftime('%Y-%m-%d  %H:%M:%S',time.localtime(time.time())) + ":  "+\
                                           err_msg + "," + \
                                           str(debug_count) + "\n")
                        self.logfile.close()
            if (self.showdata.appFrame.tab == 4):
                # self.statusbar.status.setstatus("节点控制")
                if (self.cmdaddress != ""):
                    if (self.cmdaddress != "0"):
                        if (int(self.cmdaddress) > 255):
                            self.statusbar.status.setdata("地址超出范围！")
                        else:
                            cmd_address = (chr(int(self.cmdaddress)))
                            cmd_cmd = (chr(int(self.cmd)))
                            self.uart.write("\xAA")
                            self.uart.write(cmd_address)
                            self.uart.write(cmd_cmd)
                            self.uart.write("\xBB")
                            buf1 = self.uart.read(1)
                            if (buf1 != ""):
                                if (ord(buf1) == 0xAA):  # 收ack
                                    self.statusbar.status.setdata('%s',
                                                                  "发送命令：" + str(self.cmdaddress) + "," + str(self.cmd))
                                    self.cmdaddress = "0"
                                else:
                                    self.Control_Resend += 1
                                    self.statusbar.status.setdata('%s', "重试：" + str(self.Control_Resend))
                                    if (self.Control_Resend == 20):
                                        self.Control_Resend = 0
                                        self.statusbar.status.setdata("发送失败！")
                                        self.cmdaddress = "0"
                                        time.sleep(0.05)
        if self.port in self.menu.opened_uart:
            self.menu.opened_uart.remove(self.port)

    def Createuart(self):
        self.uart = serial.Serial(timeout=2)
        self.uart.port = self.port
        self.uart.baudrate = self.baud

    def SendCmd(self, address, cmd):
        self.cmdaddress = address
        self.cmd = cmd
