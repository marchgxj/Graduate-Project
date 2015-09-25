# coding=utf-8
import time
import threading
import urllib2
import urllib

import serial


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
        self.netreceive = threading.Thread(target=self.netupdate)
        self.netreceive.setDaemon(True)
        self.threadstartflag = 0
        self.datatoshow = ''
        self.content = {}
        self.timeoutflag = 0
        self.netuploadthread = threading.Thread(target=self.netupload)
        self.netuploadthread.setDaemon(True)
        self.killthread = False
        self.thread_stop = False
        self.notedata = []
        self.longdata = [1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0]
        self.longdatastr = ""

    def netupdate(self):
        '''
        Parameter：
            
        Function：
                               将数据上传至服务器，每秒上传一次
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        data = {}
        while (1):
            if (self.killthread == True):
                break
            data["relay_id"] = "0086-110108-00022105-01"
            data["data"] = "0086-110108-00022105-0001|1,0086-110108-00022105-0002|1"
            data["park_id"] = "22105"
            post_data = urllib.urlencode(data)
            urllib2.urlopen("http://123.57.37.66:8080/sensor/post/status", post_data)
            response = urllib2.urlopen("http://123.57.11.98:8080/mm/get_new", timeout=10)
            content = eval(response.read())
            self.content = content
            time.sleep(1)

    def netupload(self):
        '''
        Parameter：
           
        Function：
                               从服务器上获取数据，并且显示网络时延
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        while 1:
            print self.longdata
            time.sleep(1)
        while (1):
            if (self.killthread == True):
                break
            start = time.clock()

            try:
                urllib2.urlopen("http://123.57.11.98:8080/mm/set_new?data=" + self.datatoshow[:-1], timeout=2)
                urllib2.urlopen("http://123.57.11.98:8080/mm/set?data=" + self.datatoshow[:-1], timeout=2)
            except:
                self.statusbar.status.setstatus('%s', "网络连接超时，请检查网络或关闭数据上传下载功能")
            end = time.clock()
            time.sleep(0.5)
            self.statusbar.status.setstatus('网络时延:%s', str(end - start))

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
        data={}
        data["relay_id"] = "0086-110108-00022105-01"
        data["park_id"] = "22105"
        data["data"]=""
        uploaddatacut=""
        self.datatoshow = ''
        # if self.uartroot.datamode == 0:
        #     self.netuploadthread.start()
        while (1):
            while (self.thread_stop == False and self.uart.isOpen() == True):
                try:
                    self.currenttab = self.showdata.appFrame.index('current')
                except:
                    print "1"

                if self.currenttab == 0:
                    if ord(self.uart.read(1)) == 0x7D:
                        if ord(self.uart.read(1)) == 0x7E:
                            count += 1
                            length = ord(self.uart.read(1))  # 读出这一包要发的节点个数
                            self.notedata = []
                            for i in range(length):


                                num = ord(self.uart.read(1)) << 8 | ord(self.uart.read(1))
                                num = num
                                num = 16
                                self.notedata.append(num)  # 偶数位为地址
                                data["data"] = data["data"]+"0086-110108-00022105-" + str(num).zfill(4) + "|"
                                self.datatoshow = self.datatoshow + str(num).zfill(4) + "|"
                                status = ord(self.uart.read(1))
                                self.notedata.append(status)  # 奇数位为数据
                                self.datatoshow = self.datatoshow + str(status) + ","
                                data["data"] = data["data"] + str(status) + ","


                            if len(self.notedata) == length * 2:
                                self.uart.write("o")
                            else:
                                print "uart data error"
                                self.uart.read(self.uart.inWaiting())  # 清空串口缓冲区内容
                            # self.statusbar.status.setdata('串口数据:%s 计数:%s', self.datatoshow[:-1], count)
                            uploaddatacut = data["data"]
                            data["data"] = uploaddatacut[:-1]
                            self.statusbar.status.setdata('串口数据:%s 计数:%s',data["data"] , count)
                            if self.stopcar.appFrame.carnum == 0:
                                self.statusbar.status.setstatus('%s', "未配置停车个数")
                            else:
                                self.stopcar.appFrame.stopcaronce(self.datatoshow[:-1])
                            self.datatoshow = ''
                            if self.uartroot.datamode == 0:
                                start = time.clock()
                                # try:
                                '''上传全部数据'''
                                # urllib2.urlopen(
                                #     "http://123.57.11.98:8080/mm/set_new?data=" + self.longdatastr[:-1],
                                #     timeout=1)
                                # urllib2.urlopen("http://123.57.11.98:8080/mm/set?data=" + self.longdatastr[:-1],
                                #                 timeout=1)
                                post_data = urllib.urlencode(data)
                                data["data"] = ""
                                response = urllib2.urlopen("http://123.57.37.66:8080/sensor/post/status", post_data,timeout=1)
                                serverresponse =  response.read()
                                serverresponsedic = eval(serverresponse)
                                # print serverresponse
                                end = time.clock()

                                self.statusbar.status.setstatus('网络延时:%s'+"  "+serverresponsedic["err_msg"], str(end - start))
                                ''''''
                                # except:
                                #     self.statusbar.status.setstatus('%s', "网络连接超时，请检查网络或关闭数据上传下载功能")

                            else:
                                if self.threadstartflag == 0:
                                    self.threadstartflag = 1
                                    self.netreceive.start()
                                comtent = self.content
                                self.netdatabuf = ''
                                try:
                                    if comtent['err_code'] == 0:
                                        for items in comtent['data']:
                                            self.netdatabuf = self.netdatabuf + str(
                                                items['name'] + '|' + items['value'] + ',')
                                        self.stopcar.appFrame.stopcaronce(self.netdatabuf[:-1])
                                        self.statusbar.status.setstatus('%s', "网络数据:" + self.netdatabuf[:-1])
                                    else:
                                        self.statusbar.status.setstatus('%s', "数据返回错误")
                                except:
                                    print "2"

                    '''旧版数据接收
                    if ord(self.uart.read(1))==0x7D:
                        if ord(self.uart.read(1))==0x7E:

                            count+=1
                            buf = self.uart.read(8)
                            self.datatoshow=''
                            for i in buf:
                                a+=1
                                if(a%2==1):
                                    self.datatoshow =  self.datatoshow+str(ord(i))+"|"
                                else:
                                    self.datatoshow =  self.datatoshow+str(ord(i))+","


                            self.statusbar.status.setdata('串口数据:%s Count:%s',self.datatoshow[:-1],count)
                    if self.stopcar.appFrame.carnum==0:
                        self.statusbar.status.setstatus('%s',"未配置停车个数")
                    else:
                        if self.carstoproot.datamode == 0:
                            self.stopcar.appFrame.stopcar(self.datatoshow[:-1])
    #                         self.statusbar.status.setstatus('%s',"未开启网络数据下载")
                        else:
                            if self.threadstartflag == 0:
                                self.threadstartflag = 1
                                self.netreceive.start()
                            comtent=self.content
                            self.netdatabuf=''
                            try:
                                if comtent['err_code'] == 0:
                                    for items in comtent['data']:
                                        self.netdatabuf = self.netdatabuf+str(items['name']+'|'+items['value']+',')
                                    self.stopcar.appFrame.stopcar(self.netdatabuf[:-1])
                                    self.statusbar.status.setstatus('%s',"网络数据:"+self.netdatabuf[:-1])
                                else:
                                    self.statusbar.status.setstatus('%s',"数据返回错误")
                            except:
                                print "2"
                    '''

    def Createuart(self):
        self.uart = serial.Serial()
        self.uart.port = self.port
        self.uart.baudrate = self.baud
