# coding=utf-8
import time
import threading
import serial
import os 
import tkMessageBox as tkmes
from test import buffer_tests
class myThread (threading.Thread):   
    def __init__(self, threadID, name, port, baud,rootframe,filename):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.thread_stop = False
        self.port = port
        self.baud = baud
        self.uart= serial.Serial(port = self.port,baudrate = self.baud)
        self.statusbar = rootframe
        self.showdata = rootframe
        self.stopcar = rootframe
        self.root = rootframe
        self.menu = rootframe.rootmenu
        self.app = self.root.appFrame
        self.carstoproot = self.menu.carstoproot
        self.data=[]
        self.datatoshow=''
        self.filename = filename
        self.file = open(self.filename,'w')
        self.radiovalue = self.app.radiovalue.get()
        self.currenttab = 0
        
    def closefile(self):
        self.file.close()
            
    def openfile(self):
        self.file = open(self.filename,'w')
    
    def beacon(self):
        return "Beacon: 需要应答:" + str((self.data[1]&0x02)>>1) + \
        "  簇ID:" + str(self.data[2])+"  簇内编号:"+str(self.data[3])+'  目的簇ID:'+str(self.data[4]) + \
        '  目的簇内编号；'+str(self.data[5]) + '  空闲负载:'+str(self.data[6])+"  RSSI:"+str(self.data[12])
    
    def unpack(self):
        type = (self.data[1]&0xFC)>>2
        if type == 1:
            return self.beacon()
        else:
            return 'unkonw type:'+str(self.data)
    
    def run(self):
        count = 0               
        while(1): 
            while(self.thread_stop == False):
#                 try:
#                     self.currenttab = self.root.appFrame.index('current')
#                 except TclError:
#                     print "3"+error
#                     self.currenttab = 2
#                 print type(self.root.appFrame.index('current'))
#                 if self.root.appFrame.index('current')==type(3):
#                     self.currenttab = self.root.appFrame.index('current')
                if self.currenttab == 2:
#                     self.uart.write("type:beacon,freenum:16")
                    buf = self.uart.read(1)
                    if len(buf)!= 0:
                        if ord(buf)==0x7D:
                            buf = self.uart.read(1)
                            if len(buf)!= 0:
                                if ord(buf)==0x7E:
                                    buf = self.uart.read(13)
                                    self.data=[]
                                    count+=1
                                    for v in buf:
                                        self.data.append(ord(v))
                                    self.statusbar.status.setdata('RX:%s Count:%s',self.data,count) 
                                    self.radiovalue = self.app.radiovalue.get()
                                    if self.radiovalue  == 0:
                                        self.showdata.appFrame.updatetext(self.unpack()+"\n")
                                    else:
                                        try:
                                            self.file.write(self.unpack()+"\n")
                                            self.file.write('RX:%s Count:%s\n' % (self.data,count))
                                        except:
                                            print "4"
                            
                            
                            
                            
#                     buf = self.uart.read(self.uart.inWaiting())
#                     if buf !="":
#                         databuf = buf
#                         count+=1
#                         self.data=[]
#                         self.statusbar.status.setdata('RX:%s Count:%s',databuf,count)
#                         for v in databuf:
#                             self.data.append(ord(v))
#                         print self.data
#                         if self.radiovalue  == 0:
#                             '''datatoshow暂时测试用'''
#                             self.showdata.appFrame.updatetext(databuf+"\n")
#                 time.sleep(0.005)

#                       
#                         
#                         pass
#                         time.sleep(0.05)
#                     else:
#                         time.sleep(0.005)
                
        
        