# coding=utf-8
__author__ = 'chang'
import sys
from Tkinter import *
import time
def tick():
    global time1
    # 从运行程序的计算机上面获取当前的系统时间
    time2 = time.strftime('%H:%M:%S')
    # 如果时间发生变化，代码自动更新显示的系统时间
    if time2 != time1:
        time1 = time2
        clock.config(text=time2)
        # calls itself every 200 milliseconds
        # to update the time display as needed
        # could use >200 ms, but display gets jerky
    clock.after(200, tick)
root = Tk()
time1 = ''
status = Label(root, text="v1.0", bd=1, relief=SUNKEN, anchor=W)
status.grid(row=0, column=0)
clock = Label(root, font=('times', 20, 'bold'))
clock.grid(row=0, column=1)
tick()
root.mainloop()
