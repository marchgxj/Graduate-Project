__author__ = 'chang'
from Tkinter import *
root = Tk()
menubar = Menu(root)

def printItem():
    print 'popup menu'

filemenu = Menu(menubar,tearoff = 0)
for k in ['Python','PHP','CPP','C','Java','JavaScript','VBScript']:
    filemenu.add_command(label = k,command = printItem)
menubar.add_cascade(label = 'Language',menu = filemenu)
def popup(event):
    menubar.post(event.x_root,event.y_root)
root.bind('<Button-3>',popup)
root.mainloop()