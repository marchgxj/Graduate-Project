# coding=utf-8
import time
import serial
from collections import deque
import platform
import os

__author__ = 'Changxiaodong'


class Relay:
    def __init__(self):
        self.VALUE_LENGTH = 50
        self.VALUE_NUM = 28
        self.value = [0 for n in range(self.VALUE_NUM)]
        try:
            if not os.path.exists("../Data/Identify"):
                os.makedirs('./Data/Identify')
        except:
            pass
        self.filepath = "./Data/Identify/" + time.strftime('%Y-%m-%d_%H-%M-%S', time.localtime(time.time())) + '.txt'
        self.framecount = 0
        self.uart = serial.Serial()



    def run(self):
        if platform.system() == "Linux":
            self.port = "/dev/ttyAMA0"
        else:
            self.port = "COM3"
        self.uart.port = self.port
        self.uart.baudrate = 115200
        self.uart.open()
        try:
            self.uart.read(self.uart.inWaiting())
        except serial.SerialException:
            print "serial inwaiting not null"
        print "running"
        buf = ""
        while 1:
            try:
                buf = self.uart.read(1)
            except serial.SerialException:
                pass
            if len(buf) != 0:
                if ord(buf) == 0x7D:
                    buf = []
                    try:
                        buf += self.uart.read(self.VALUE_LENGTH)
                    except serial.SerialException:
                        pass
                    self.file = open(self.filepath, "a+")
                    self.file.write("|" + str(time.time()) + "|")
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
                    self.value[26] = (ord(buf[46]) << 8 | ord(buf[47]))
                    self.value[27] = (ord(buf[48]) << 8 | ord(buf[49]))

                    for i in range(self.VALUE_NUM):
                        self.file.write(str(self.value[i]) + " ")
                    self.file.write("\n")

                    self.file.close()
                    try:
                        self.uart.read(self.uart.inWaiting())
                    except serial.SerialException:
                        pass
                    self.framecount += 1
                    print self.framecount



if __name__ == '__main__':
    relay = Relay()
    relay.run()
