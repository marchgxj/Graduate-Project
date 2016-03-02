# coding=utf-8
import time
import threading
import serial
import datetime
import os 
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
        if not os.path.exists("../Sniffer"):
            os.makedirs('../Sniffer')
        self.filename = filename
        with open(filename, 'w') as file:
            file.write("Sniffer Start at:" + time.strftime('%Y-%m-%d  %H:%M:%S',time.localtime(time.time())) + "\n")
        self.radiovalue = self.app.radiovalue.get()
        self.currenttab = 0
        self.dataintext = ""
        self.dataintextbuf=""
        self.datatype = 0
        
    def openuart(self):
        '''
        Parameter：
           
        Function：
                               打开串口
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.uart= serial.Serial(port = self.port,baudrate = self.baud) 
           
    def closefile(self):
        '''
        Parameter：
           
        Function：
                               关闭文件
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.file.close()
            
    def openfile(self):
        '''
        Parameter：
           
        Function：
                               打开文件
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        self.file = open(self.filename,'w')
    
    def beacon(self):
        '''
        Parameter：
           
        Function：
                               获取beacon内容
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        return "Beacon: 应答:" + str((self.data[1]&0x02)>>1) + \
            "  能耗:" + str(self.data[1]&0x01)+\
            "  目的簇ID:" + str(self.data[2])+\
            "  目的编号:"+str(self.data[3])+\
            '  簇ID:'+str(self.data[4]) + \
            '  编号；'+str(self.data[5]) + \
            '  空闲负载:'+str(self.data[6])+\
            '  校验:'+str(self.data[7])+\
            "  RSSI:"+str(self.data[12])
            
    def JoinRequest(self):
        '''
        Parameter：
           
        Function：
                               获取JoinRequest内容
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        return "JoinRQ: 应答:" + str((self.data[1]&0x02)>>1)+\
             "  目的簇ID:" + str(self.data[2]) + \
             "  目的编号:" + str(self.data[3]) + \
             "  物理地址:" + str(self.data[4]) + str(self.data[5])+\
             '  校验:'+str(self.data[6])
           
    def JoinRequestACK(self):
        '''
        Parameter：
           
        Function：
                               获取JoinRequestACK内容
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        return "JoinRQACK:应答:"+ str((self.data[1]&0x02)>>1) + \
            "  接受:"+str(self.data[1]&0x01) +\
            "  目的地址:" + str(self.data[2]) + str(self.data[3]) + \
            "  源簇ID:"+str(self.data[4])+\
            "  源编号:"+str(self.data[5])+\
            "  分配簇ID:"+str(self.data[6])+\
            "  分配编号:" + str(self.data[7])+\
            '  校验:'+str(self.data[8])
    
    def JoinRequestACKOK(self):
        '''
        Parameter：
           
        Function：
                               获取JoinRequestACKOK内容
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        return "JoinRQACKOK:应答:" + str((self.data[1]&0x02)>>1)+\
            "  目的簇ID:"+str(self.data[2])+\
            "  目的编号:"+str(self.data[3])+\
            "  源簇ID:" + str(self.data[4])+\
            "  源编号:"+str(self.data[5])
                 
    def Data(self):
        '''
        Parameter：
           
        Function：
                               获取Data内容
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        return "Data: 应答:" + str((self.data[1]&0x02)>>1)+\
            "模式:"+str(self.data[1]&0x01)+\
            "  目的簇ID:"+str(self.data[2])+\
            "  目的编号:"+str(self.data[3])+\
            "  源簇ID:" + str(self.data[4])+\
            "  源编号:"+str(self.data[5])+\
            "  绝对时隙号:"+str(self.data[6]<<8|self.data[7])+\
            "  数据:"+str(self.data[8])+\
            '  校验:'+str(self.data[9])+\
            '  ACK丢失数量:'+str(self.data[10]<<8|self.data[11])

    def DataACK(self):
        '''
        Parameter：
           
        Function：
                               获取DataACK内容
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        return "DataACK: 应答:" + str((self.data[1]&0x02)>>1)+\
            "  重新入网:"+str(self.data[1]&0x01)+\
            "  目的簇ID:"+str(self.data[2])+\
            "  目的编号:"+str(self.data[3])+\
            "  源簇ID:" + str(self.data[4])+\
            "  源编号:"+str(self.data[5])+\
            "  时间戳:"+str(self.data[6]<<8|self.data[7])+\
            "  控制命令:"+str(self.data[8])+\
            '  校验:'+str(self.data[9])
            
    def ReJoin(self):
        return "ReJoin: 应答:" + str((self.data[1]&0x02)>>1)+\
            "  目的簇ID:"+str(self.data[2])+\
            "  目的编号:"+str(self.data[3])+\
            "  源簇ID:" + str(self.data[4])+\
            "  源编号:"+str(self.data[5])+\
            "  目的物理地址:"+str(self.data[6]<<8|self.data[7])+\
            '  校验:'+str(self.data[8])

    def unpack(self):
        '''
        Parameter：
           
        Function：
                               获取数据包类型，调用对应数据包拆包函数
        Autor:xiaoxiami 2015.5.29
        Others：
        '''
        type = (self.data[1]&0xFC)>>2
        if type == 1:
            self.datatype = 1
            return self.beacon()
        elif type == 2:
            self.datatype = 2
            return self.JoinRequest()
        elif type == 3:
            self.datatype = 3
            return self.JoinRequestACK()
        elif type == 4:
            self.datatype = 4
            return self.JoinRequestACKOK()
        elif type == 5:
            self.datatype = 5
            return self.Data()
        elif type == 6:
            self.datatype = 6
            return self.DataACK()
        elif type == 7:
            self.datatype = 7
            return self.ReJoin()
        else:
            return 'unkonw type:'+str(self.data)
    
    def run(self):
        '''
        Parameter：
           
        Function：
                               接收串口数据，拆包，显示
        Autor:xiaoxiami 2015.5.29
        Others：线程
        '''
        self.count = 0
        self.showdata.appFrame.updatetext()
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
                self.currenttab = self.app.tab
                if self.currenttab == 1:
                    buf = self.uart.read(1)
                    if len(buf)!= 0:
                        if ord(buf)==0x7D:
                            buf = self.uart.read(1)
                            if len(buf)!= 0:
                                if ord(buf)==0x7E:
                                    buf = self.uart.read(13)
                                    self.data=[]
                                    self.count+=1
                                    for v in buf:
                                        self.data.append(ord(v))
                                    self.statusbar.status.setdata('RX:%s Count:%s',self.data,self.count) 
                                    self.radiovalue = self.app.radiovalue.get()
                                    self.unpack()
                                    
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
                                    self.count+=1
                                    for v in buf:
                                        self.data.append(ord(v))
                                    self.statusbar.status.setdata('RX:%s Count:%s',self.data,self.count) 
                                    self.radiovalue = self.app.radiovalue.get()
                                    if self.radiovalue  == 0:
#                                         try:
#                                             self.showdata.appFrame.updatetext(str(count)+"--"+self.unpack()+"\n")
#                                         except:
#                                             print "update error"
                                        self.dataintext = self.unpack()
                                        format = '%-3d'
                                        localtime = time.strftime('%H:%M:%S', time.localtime(time.time())) + "::" +(format % (datetime.datetime.now().microsecond/1000))  #str('%3f' % datetime.datetime.now().microsecond/1000)
                                        self.dataintextbuf=localtime+"--"+self.dataintext+"\n"+self.dataintextbuf
#                                         print str(self.count)+"--"+self.dataintext
                                        with open(self.filename, 'w') as file:
                                            file.write(self.dataintext+"\n")
                                            file.write('RX:%s Count:%s\n' % (self.data,self.count))
                                        if self.count % 2000 == 0:
                                            self.showdata.appFrame.cleartext()
                                    else:
                                        try:
                                            self.file.write(self.dataintext+"\n")
                                            self.file.write('RX:%s Count:%s\n' % (self.data,self.count))
                                        except:
                                            print "4"
   
    def updatetext(self):
        '''
        Parameter：

        Function：
                               定时更新数据
        Autor:xiaoxiami 2015.5.29
        Others：线程
        '''

        while(1):
            if self.currenttab == 2:
                self.showdata.appFrame.updatetext(self.dataintextbuf,self.datatype)
                self.dataintextbuf=""
            time.sleep(0.5)
