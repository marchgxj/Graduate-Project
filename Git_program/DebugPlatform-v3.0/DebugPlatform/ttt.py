from Tkinter import *
 
def onGo():        
    def counter(i):        

        t.insert(END,'a_'+str(i))
        t.after(10, counter, i-1)
    counter(50)
                 
root = Tk()
t = Text(root)
t.pack()
goBtn = Button(text = "Go!",command = onGo)
goBtn.pack()
root.mainloop()