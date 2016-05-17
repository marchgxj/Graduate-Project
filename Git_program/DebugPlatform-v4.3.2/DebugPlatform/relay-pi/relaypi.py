# coding=utf-8
import time
import urllib2
import urllib
import serial
import threading
from collections import deque
import platform

__author__ = 'Changxiaodong'


class DataStruct:
    node_data = {}
    node_data["relay_id"] = "0086-110108-00022105-01"
    node_data["park_id"] = "22105"
    node_data["data"] = ""
    post_data = ""
    upload_data_quene = deque(maxlen=2)


class Relay:
    def __init__(self):
        self.OFFSET = 0
        self.data = DataStruct()
        self.command_to_node = {}

    def run(self):
        '''
        Parameter：

        Function：
                从串口接收到数据后上传至服务器
        Autor:xiaoxiami 2015.5.29
        Others：时间长会出错。可以修改，当数据与当前界面显示数据发生变化时在更新界面
        '''

        DATA_PACKET = 1
        DEBUG_PACKET = 2

        def initUartDependOnSystem():
            uart = serial.Serial()
            if platform.system() == "Linux":
                port = "/dev/ttyAMA0"
            else:
                port = "COM6"

            uart.port = port
            uart.baudrate = 115200
            uart.open()
            return uart

        def initLogDependOnSystem():
            import os
            if not os.path.exists("./Log/"):
                os.makedirs('./Log/')

            if platform.system() == "Linux":
                data_log_path = "./Log/" + time.strftime('%Y-%m-%d_%H-%M-%S', time.localtime(time.time())) + '.txt'
            else:
                data_log_path = "./Log/" + time.strftime('%Y-%m-%d_%H-%M-%S', time.localtime(time.time())) + '.txt'
            err_log_path = "./Log/log.txt"

            with open(err_log_path, "w") as f:
                f.write(
                    "Program Start at:" + time.strftime('%Y-%m-%d  %H:%M:%S', time.localtime(time.time())) + "\n")
            print "running"

            log = {}
            log["data_log"] = data_log_path
            log["err_log"] = err_log_path

            return log

        def writeErrlog(path, err):
            with open(path, "a") as f:
                f.write(str(err) + "\n")

        def getPacketType():
            try:
                head_str = ord(self.uart.read(1))
            except serial.SerialException, err:
                writeErrlog(self.logpath["err_log"], err)
                return False
            if head_str == 0x7E:
                return DATA_PACKET
            elif head_str == 0x7F:
                return DEBUG_PACKET
            return 0

        def isPacketHead():
            try:
                head_str = self.uart.read(1)
            except serial.SerialException, err:
                writeErrlog(self.logpath["err_log"], err)
                return False
            if head_str != "":
                if ord(head_str) == 0x7D:
                    return True
            return False

        def getPacketData():

            def getPacketLength():
                try:
                    length = ord(self.uart.read(1))
                    return length
                except serial.SerialException, err:
                    writeErrlog(self.logpath["err_log"], err)
                    return 0

            def getNodenumandStatus():
                try:
                    num = ord(self.uart.read(1)) << 8 | ord(self.uart.read(1))
                    status = ord(self.uart.read(1))
                    return num, status
                except serial.SerialException, err:
                    writeErrlog(self.logpath["err_log"], err)
                    return 0, 0

            def writeDatalog(num, data):
                if num in data_dic:
                    if data != data_dic[num]:
                        with open(self.logpath["data_log"], "a+") as file:
                            file.write(
                                str(num) +
                                time.strftime(
                                    ' %Y-%m-%d %H:%M:%S',
                                    time.localtime(time.time())
                                ) + " " +
                                str(data) + "\n"
                            )
                        data_dic[num] = data
                else:
                    data_dic[num] = data

            def pushToUploadQuene(data):
                self.data.node_data["data"] = data
                self.data.upload_data_quene.append(urllib.urlencode(self.data.node_data))

            def getDataandPushToUploadQuene(length):
                printdata = {}
                data = ""
                for i in range(length):
                    node_num, status = getNodenumandStatus()
                    if node_num != 0:
                        writeDatalog(node_num, status)
                        printdata[node_num] = status
                        data = data + "0086-110108-00022105-" + str(node_num).zfill(4) + "|" \
                               + str(status) + ','
                # print printdata
                pushToUploadQuene(data[:-1])

            def clearUartInwaiting():
                self.uart.read(self.uart.inWaiting())

            packet_length = getPacketLength()
            getDataandPushToUploadQuene(packet_length)
            clearUartInwaiting()

        def getDebugInfo(uart):
            err_msg = uart.readline()
            return err_msg[:-1]

        def sendCommand(add, cmd):
            '''
            Parameter：add:address of node
                       cmd:command num

            Function：
                                  向节点发送控制信息
            Autor:xiaoxiami 2015.12.26
            Others：控制信息可以由上位机输入，或从服务器上获取
            '''
            cmd_address = (chr(int(add) - self.OFFSET))
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

        def sendCommandfromServer():
            '''
            Parameter：

            Function：
                                  从服务器获取控制命令，发送给中继
            Autor:xiaoxiami 2015.12.26
            Others：
            '''

            if not self.command_to_node:
                return
            for key, value in self.command_to_node.items():
                add = int(str(key).split("-")[-1])
                i = 0
                for i in range(10):
                    print i
                    result = sendCommand(add, value)
                    print result
                    if result:
                        break
                if i == 9:
                    print "send command fail"
                    with open(self.logpath["err_log"], "a+") as file:
                        file.write(time.strftime('%Y-%m-%d  %H:%M:%S',
                                                 time.localtime(
                                                     time.time())) + ":  命令发送失败:" + key + "," + value + "\n")

            self.command_to_node = {}

        data_dic = {}

        self.uart = initUartDependOnSystem()
        self.logpath = initLogDependOnSystem()
        netUploadThread = threading.Thread(target=self.dataUpload)
        netUploadThread.setDaemon(True)
        netUploadThread.start()

        while 1:
            if isPacketHead():
                if getPacketType() == DATA_PACKET:
                    getPacketData()
                    sendCommandfromServer()
                    # print self.data.node_data["data"]
                elif getPacketType() == DEBUG_PACKET:
                    err = getDebugInfo()
                    writeErrlog(self.logpath["err_log"], err)

    def dataUpload(self):
        '''
        Parameter：

        Function：
                               将数据上传至服务器
        Autor:xiaoxiami 2015.12.3
        Others：
        '''

        def initNetlogFile(filename):
            with open(filename, "w") as f:
                f.write("running...")

        def initLedUnderLinux():
            if platform.system() == "Linux":
                GPIO.setmode(GPIO.BOARD)
                GPIO.setup(12, GPIO.OUT)

        def ledHigh():
            if platform.system() == "Linux":
                GPIO.output(12, GPIO.HIGH)

        def ledLow():
            if platform.system() == "Linux":
                GPIO.output(12, GPIO.LOW)

        def ledToggle(count):
            if platform.system() == "Linux":
                if count % 2 == 0:
                    GPIO.output(12, GPIO.HIGH)
                else:
                    GPIO.output(12, GPIO.LOW)

        def postDataToServer(postdata):
            try:
                response = urllib2.urlopen("http://www.xiaoxiami.space/info/post/", postdata, timeout=2)
                feedback = eval(response.read())
                return feedback
            except urllib2.HTTPError, err:
                writeNetLog(net_log_filename, err)
                flashLed()
                print err
            except urllib2.URLError, err:
                writeNetLog(net_log_filename, err)
                flashLed()
                print err
            except:
                flashLed()
            finally:
                while len(self.data.upload_data_quene) > 0:
                    self.data.upload_data_quene.pop()
            return ""

        def writeNetLog(filename, delay):
            try:
                if float(delay) > 1:
                    with open(filename, "a") as f:
                        f.write(str(delay) + "\n")
            except:
                with open(filename, "a") as f:
                    f.write(str(delay) + "\n")

        def flashLed():
            if platform.system() == "Linux":
                GPIO.output(12, GPIO.LOW)
                time.sleep(0.05)
                GPIO.output(12, GPIO.HIGH)
                time.sleep(0.05)
                GPIO.output(12, GPIO.LOW)
                time.sleep(0.05)
                GPIO.output(12, GPIO.HIGH)
                time.sleep(0.05)
                GPIO.output(12, GPIO.LOW)
                time.sleep(0.05)
                GPIO.output(12, GPIO.HIGH)
                time.sleep(0.05)
                GPIO.output(12, GPIO.LOW)
                time.sleep(0.05)
                GPIO.output(12, GPIO.HIGH)
                time.sleep(0.05)
                GPIO.output(12, GPIO.LOW)
                time.sleep(0.05)
                GPIO.output(12, GPIO.HIGH)

        def isEmptyUploadbuf():
            return len(self.data.upload_data_quene)

        def getCommandFromFeedback(feedback):
            if isinstance(feedback, dict):
                command = {}
                for key, value in feedback.items():
                    if key != "Status":
                        command[key] = value
                return command

        if platform.system() == "Linux":
            import RPi.GPIO as GPIO

        net_log_filename = "./Log/net_log.txt"
        initNetlogFile(net_log_filename)
        initLedUnderLinux()
        count = 0

        while True:
            if isEmptyUploadbuf() != 0:
                post_data = self.data.upload_data_quene.popleft()
                now = time.time()

                ledToggle(count)
                count += 1
                if count > 10:
                    count = 0

                feedback = postDataToServer(post_data)
                print feedback
                receive = time.time()
                delay = receive - now
                writeNetLog(net_log_filename, delay)
                self.command_to_node = getCommandFromFeedback(feedback)


            else:
                time.sleep(0.1)


if __name__ == '__main__':
    relay = Relay()
    relay.run()
