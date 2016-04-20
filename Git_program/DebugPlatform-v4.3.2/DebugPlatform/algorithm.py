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
        self.angle = []
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
        z = []

        if source == "all":
            for data in filedata:
                x.append(data[0])
                y.append(data[1])
                z.append(data[2])
            return x, y, z

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
        if not self.sourceCheck(source) or not filedata:
            return 1
        x, y = self.getdataFromSource(filedata, source)
        diameter = 1


        # start caculate
        startx = x[0]
        starty = y[0]
        max_x = 0
        max_y = 0
        for i in range(len(x)):
            max_x = max(max_x, abs(x[i]-startx))
            if max_x>15000:
                print x[i]
            max_y = max(max_y, abs(y[i]-starty))
            #diameter = max(diameter, (math.sqrt(((startx - x[i]) ** 2) + ((starty - y[i]) ** 2))))
        print "------------"
        print max_x,max_y
        print "------------"
        return max_y

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
        for i in range(len(x) - 1):
            perimeterbuf = math.sqrt((x[i] - x[i + 1]) ** 2 + (y[i] - y[i + 1]) ** 2)
            if (perimeterbuf > 20):  # 距离大于15才认为是有效周长，避免原地抖动对周长的增加
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
        perimeter = self.getPerimeter(filedata, source)
        diameter = self.getMaxDiameter(filedata, source)
        compatness = perimeter ** 2 / diameter
        print perimeter,diameter,compatness
        return compatness

    def getAngle(self, filedata, source):
        '''
        Parameter：filedata:三个坐标轴数据
                   source：坐标轴选择
        Function：
                   计算图像与XOY平面夹角
        Autor:xiaoxiami 2016.1.20
        Others：
        '''

        if len(filedata) == 0:
            return 0

        x, y, z = self.getdataFromSource(filedata, "all")

        # start caculate
        prex = x[0]
        prey = y[0]
        prez = z[0]

        postion_a = [prex, prey, prez]
        postion_b = []
        postion_c = []
        angle = []

        count = 1
        times = 0

        for i in range(len(x)):
            if math.sqrt(pow(x[i] - prex, 2) + pow(y[i] - prey, 2) + pow(z[i] - prez, 2)) > 100:
                prex = x[i]
                prey = y[i]
                prez = z[i]
                if count % 3 == 1:
                    postion_b = [prex, prey, prez]
                elif count % 3 == 2:
                    postion_c = [prex, prey, prez]
                    a = postion_c[0] - postion_b[0]
                    b = postion_c[1] - postion_b[1]
                    m = postion_c[2] - postion_b[2]
                    c = postion_c[0] - postion_a[0]
                    d = postion_c[1] - postion_a[1]
                    n = postion_c[2] - postion_a[2]

                    if not (a * d == b * c or a * n == m * c):
                        line_a = [int(a), int(b), int(m)]
                        line_b = [int(c), int(d), int(n)]

                        nomal_vector = self.getNomalVector(line_a, line_b)
                        anglebuf = math.acos(nomal_vector[0] / self.getVectorNorm(nomal_vector)) * 180 / 3.1415
                        # if anglebuf > 90:
                        #     anglebuf -= 90
                        # 法向量与[1,0,0]
                        angle.append(anglebuf)
                elif count % 3 == 0:
                    postion_a = [prex, prey, prez]
                count += 1
        print angle
        print self.classsificationAngle(angle)
        print "\n"

        return 0

    def classsificateAngle(self, angle):
        '''
        Parameter：angle:包含角度的数组

        Function：
                   从0-90度每个区间的数量
        Autor:xiaoxiami 2016.1.20
        Others：
        '''
        if not angle:
            return 0
        ave = 0
        count = [0 for i in range(20)]
        for a in angle:
            ave += a
            count[int(a / 10)] += 1
        return count, ave / len(angle)

    def getVectorNorm(self, vector):
        '''
        Parameter：vector:三维向量

        Function：
                   获取向量的模
        Autor:xiaoxiami 2016.1.20
        Others：
        '''
        return math.sqrt(pow(vector[0], 2) + pow(vector[1], 2) + pow(vector[2], 2))

    def getEuclideanMetric(self, data1, data2):
        '''
        Parameter：data1:点1[x,y,z]
                   data2:点2[x,y,z]

        Function：
                   两个点的欧式距离
        Autor:xiaoxiami 2016.1.21
        Others：
        '''
        return int(math.sqrt(pow(data1[0]-data2[0], 2) + pow(data1[1]-data2[1], 2) + pow(data1[2]-data2[2], 2)))

    def getNomalVector(self, vector1, vector2):
        '''
        Parameter：vector1:平面上的一个向量
                   vector2：平面上的一个向量
        Function：
                   获取平面法向量
        Autor:xiaoxiami 2016.1.20
        Others：
        '''
        x = float((vector1[2] * vector2[1] - vector1[1] * vector2[2])) / \
            float((vector1[1] * vector2[0] - vector1[0] * vector2[1]))
        y = float((vector1[2] * vector2[0] - vector1[0] * vector2[2])) / \
            float((vector1[0] * vector2[2] - vector1[2] * vector2[0]))
        z = 1
        return [x, y, z]

    def simulateGetData(self, filedata):
        if not filedata:
            return 0
        middle = [filedata[0][0], filedata[0][1], filedata[0][2]]
        # middle = [2578, 1728, 1813]
        finaldata = []
        start_flag = 0
        end_sum = 0
        end_count = 0

        for i in range(1,len(filedata)):
            if start_flag == 0:
                if self.getEuclideanMetric(filedata[i-1], filedata[i]) > 20:
                    start_flag = 1
                    print "start"
            if start_flag == 1:
                end_distance = self.getEuclideanMetric(filedata[i-1],filedata[i])
                if end_distance > 20:
                    finaldata.append(filedata[i])

                if end_distance < 15:
                    end_sum += end_distance
                    end_count += 1
                else:
                    end_count = 0
                    end_sum = 0
                if end_count > 50 and (end_sum/end_count) < 15:
                    start_flag = 0
                    print "stop"
        print len(finaldata), len(filedata)
        print self.getCompatness(finaldata, self.settings["source"]),self.getCompatness(filedata, self.settings["source"])

    def cfModel(self, variance, extremum, distance, compatness):
        pass

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
            if self.settings["method"] == 4:
                self.angle.append(int(self.getAngle(filedata, self.settings["source"])))
            if self.settings["method"] == 5:
                self.simulateGetData(filedata)

        if data_process_success:
            if "dot_animate" in self.settings["display"]:
                os.system("python drawdot.py %s %s %s" % (self.settings["period"], self.settings["method"], "animate"))
            elif "dot_static" in self.settings["display"]:
                os.system("python drawdot.py %s %s %s" % (self.settings["period"], self.settings["method"], "static"))
