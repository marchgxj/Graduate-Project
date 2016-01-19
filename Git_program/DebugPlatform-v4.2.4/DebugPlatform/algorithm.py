# coding=utf-8
__author__ = 'ChangXiaodong'
import threading
import os
import math


class Algotithm():
    def __init__(self, settings, parent):
        self.settings = settings
        self.parent = parent
        self.period = float(self.settings["period"]) / 1000
        self.diameter = []
        self.perimeter = []
        self.compatness = []
        # self.parent.root.status.setdata(settings["period"])

        if not os.path.exists("../Data/Draw/"):
            os.makedirs('../Data/Draw/')
        if os.path.exists("../Data/Draw/tangentCoordinate.txt"):
            os.remove("../Data/Draw/tangentCoordinate.txt")
        self.algotithm = threading.Thread(target=self.run)
        self.algotithm.setDaemon(True)
        self.algotithm.start()

    def saveData(self, x, y, name):
        '''
        Parameter：x:[1,2,3....]
                   y:[1,2,3....]
                   name：文件名称，一般为算法名称
        Function：将数据存储在txt中，供绘图使用。
        Autor:xiaoxiami 2015.12.27
        Others：
        '''
        with open("../Data/Draw/%s" % name, 'a+') as file:
            for i in x:
                file.write(str(i) + " ")
            file.write("|")
            for i in y:
                file.write(str(i) + " ")
            file.write("\n")

    def sourceCheck(self, source):
        if source[0] == "1" and source[1] == "1" and source[2] == "1":
            self.parent.root.status.setdata("绘制散点图不能同时选择三个轴")
            return 0
        if not source[0] == "1" and not source[1] == "1" and not source[2] == "1":
            self.parent.root.status.setdata("没有选择数据源")
            return 0
        if source[0] == "1" and source[1] == "0" and source[2] == "0":
            self.parent.root.status.setdata("绘制散点图不能只选择一个轴")
            return 0
        if source[0] == "0" and source[1] == "1" and source[2] == "0":
            self.parent.root.status.setdata("绘制散点图不能只选择一个轴")
            return 0
        if source[0] == "0" and source[1] == "0" and source[2] == "1":
            self.parent.root.status.setdata("绘制散点图不能只选择一个轴")
            return 0
        return 1

    def getdataFromSource(self, filedata, source):
        x = []
        y = []

        if source[0] == "1" and source[1] == "1":
            for data in filedata:
                x.append(data[0])
                y.append(data[1])
        elif source[0] == "1" and source[2] == "1":
            for data in filedata:
                x.append(data[0])
                y.append(data[2])
        elif source[1] == "1" and source[2] == "1":
            for data in filedata:
                x.append(data[1])
                y.append(data[2])

        return x, y

    def tangentCoordinate(self, filedata, source):
        distance = 0
        x_far = 0
        y_far = 0
        plotx = []
        ploty = []
        count = 0

        nodex = []
        nodey = []

        if not self.sourceCheck(source):
            return 0

        for i in range(len(filedata)):
            data = filedata[i]
            if source[0] == "1" and source[1] == "1":
                x = data[0]
                y = data[1]
            elif source[0] == "1" and source[2] == "1":
                x = data[0]
                y = data[2]
            elif source[1] == "1" and source[2] == "1":
                x = data[1]
                y = data[2]
            j = i
            distance = 0
            while distance < 10 or x_far == x:
                x_far = filedata[j][1]
                y_far = filedata[j][2]
                distance = ((x_far - x) ** 2 + (y_far - y) ** 2) ** 0.5
                j += 1
                if j > len(filedata) - 1:
                    break
            if x_far - x != 0:
                slot = float((y_far - y)) / float((x_far - x))
                plotx.append(distance)
                ploty.append(slot)
                count += 1

        self.saveData(plotx, ploty, "tangentCoordinate.txt")
        return 1

    def getMaxDiameter(self, filedata, source):
        '''
        Parameter：filedata:三个坐标轴数据
                   source：坐标轴选择
        Function：
                   计算每个点到起始点的最大距离，近似当做图像面积
        Autor:xiaoxiami 2016.1.16
        Others：
        '''
        if not self.sourceCheck(source):
            return 0
        x, y = self.getdataFromSource(filedata, source)
        diameter = 1


        # start caculate
        startx = x[0]
        starty = y[0]
        for i in range(len(x)):
            diameter = max(diameter, (math.sqrt(((startx - x[i]) ** 2) + ((starty - y[i]) ** 2))))
        return diameter

    def getPerimeter(self, filedata, source):
        '''
        Parameter：filedata:三个坐标轴数据
                   source：坐标轴选择
        Function：
                   计算图像周长
        Autor:xiaoxiami 2016.1.16
        Others：
        '''
        if not self.sourceCheck(source):
            return 0
        x, y = self.getdataFromSource(filedata, source)
        perimeter = 0

        # start caculate
        for i in range(len(x)-1):
            perimeterbuf = math.sqrt((x[i]-x[i+1])**2 + (y[i]-y[i+1])**2)
            if (perimeterbuf > 15):         #距离大于15才认为是有效周长，避免原地抖动对周长的增加
                perimeter += perimeterbuf
        return perimeter

    def getCompatness(self, filedata, source):
        '''
        Parameter：filedata:三个坐标轴数据
                   source：坐标轴选择
        Function：
                   计算图像紧致度
                   紧致度 = 周长**2 / 面积
        Autor:xiaoxiami 2016.1.16
        Others：XZ轴数据最明显，能够区分停车和临车干扰
        '''
        if not self.sourceCheck(source):
            return 0

        # start caculate
        compatness = self.getPerimeter(filedata, source)**2 / self.getMaxDiameter(filedata, source)
        return compatness



    def run(self):
        data_process_success = 0
        for filedata in self.settings["data"]:
            if self.settings["method"] == 0:
                data_process_success = self.tangentCoordinate(filedata, self.settings["source"])
            if self.settings["method"] == 1:
                self.diameter.append(int(self.getMaxDiameter(filedata, self.settings["source"])))
            if self.settings["method"] == 2:
                self.perimeter.append(int(self.getPerimeter(filedata, self.settings["source"])))
            if self.settings["method"] == 3:
                self.compatness.append(int(self.getCompatness(filedata, self.settings["source"])))

        if data_process_success:
            if "dot_animate" in self.settings["display"]:
                os.system("python drawdot.py %s %s %s" % (self.settings["period"], self.settings["method"], "animate"))
            elif "dot_static" in self.settings["display"]:
                os.system("python drawdot.py %s %s %s" % (self.settings["period"], self.settings["method"], "static"))
