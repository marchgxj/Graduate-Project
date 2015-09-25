# coding=utf-8
import time
import urllib2
import urllib
import serial
import RPi.GPIO as GPIO 
__author__ = 'Changxiaodong'
class Relay:
    def run(self):
        '''
        Parameter：

        Function：
                              从串口数据中获取停车信息，并且更新界面
        Autor:xiaoxiami 2015.5.29
        Others：时间长会出错。可以修改，当数据与当前界面显示数据发生变化时在更新界面
        '''
		GPIO.setmode(GPIO.BOARD) 
		GPIO.setup(4,GPIO.OUT)
		
        self.uart = serial.Serial()
        self.port = "/dev/ttyAMA0"
        self.uart.port = self.port
        self.uart.baudrate = 115200
        self.uart.open()
        print "running"
		GPIO.output(4, GPIO.HIGH) 
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
        while(1):
            if ord(self.uart.read(1)) == 0x7D:
                if ord(self.uart.read(1)) == 0x7E:
                    count += 1
                    length = ord(self.uart.read(1))  # 读出这一包要发的节点个数
                    self.notedata = []
                    for i in range(length):
                        num = ord(self.uart.read(1)) << 8 | ord(self.uart.read(1))
                        num = num
                        self.notedata.append(num)  # 偶数位为地址
                        data["data"] = data["data"]+"0086-110108-00022105-" + str(num).zfill(4) + "|"
                        status = ord(self.uart.read(1))
                        self.notedata.append(status)  # 奇数位为数据
                        data["data"] = data["data"] + str(status) + ","
                    if len(self.notedata) == length * 2:
                        self.uart.write("o")
                    else:
                        print "uart data error"
                        self.uart.read(self.uart.inWaiting())  # 清空串口缓冲区内容

                    uploaddatacut = data["data"]
                    data["data"] = uploaddatacut[:-1]
                    print uploaddatacut[:-1]
                    start = time.clock()
                    try:
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
                        eval(serverresponse)
                        # print serverresponse
                        end = time.clock()
                        print "time:" + str(end - start)
                        ''''''
                    except:
                        pass


if __name__ == '__main__':
    relay = Relay()
    relay.run()



