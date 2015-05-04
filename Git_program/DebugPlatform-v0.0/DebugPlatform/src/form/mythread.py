# coding=utf-8
import time
import threading
import serial
import os 
class myThread (threading.Thread):   
    
    def __init__(self, threadID, name, port, baud,rootframe,filename):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.thread_stop = True
        self.port = port
        self.baud = baud
        self.uart= serial.Serial(port = self.port,baudrate = self.baud)
        self.statusbar = rootframe
        self.showdata = rootframe
        self.count = 0
        self.filename = filename
        
        if os.path.exists(self.filename):
            os.rmdir(self.filename)
        self.file = open(self.filename,'w')
        self.radiovalue = 0
        
        
    def run(self):                   
        print "thread " + self.name
        currenttab = 2
        while(1):
            while(self.thread_stop == False):
                self.uart.write("hello this is my fisrt uart")
                self.count = self.count + 1
                databuf = self.uart.read(self.uart.inWaiting())
                showbuf = 'RX:'+ databuf + "    count:" + str(self.count)
                
                self.statusbar.status.setdata('%s',showbuf)
                try:
                    currenttab = self.showdata.appFrame.index('current')
                except:
                    currenttab = 2

#                 print self.showdata.appFrame.tabs()
                self.radiovalue  = self.showdata.appFrame.radiovalue.get()
                if(currenttab == 2):
                    if self.radiovalue  == 0:
                        self.showdata.appFrame.updatetext(databuf+"\n")
                        self.file.write(showbuf+'\n')
                        time.sleep(0.05)
                    elif self.radiovalue  == 1:
                        self.file.write(showbuf+'\n')
                        time.sleep(0.005)
                else:
                    time.sleep(0.05)
#                     print "%s-%s" % ('data', time.strftime('%Y-%m-%d_%H-%M-%S',time.localtime(time.time())))
                
    def closeuart(self):
        self.uart.close()
        
    def clsoefile(self):
        self.file.close()
        
    def openfile(self):
        self.file = open(self.filename,'w')
    




