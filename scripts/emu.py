#!/usr/bin/python
# sudo apt-get install python-tk python-imaging python-imaging-tk
import mmap
import os
import os.path
import struct
import Tkinter
import Image
import ImageTk
import copy
import getopt
import sys
import struct
import tkFileDialog
KindleImgPath='/DuoKan/fb_shmem'
KindleKeyPath='/DuoKan/key_fifo'
KindleTouchPath='/DuoKan/touch_fifo'

SCALE=1

#print struct.unpack('i', KindleImg[-4:])

KEY_Q=16
KEY_W=17
KEY_E=18
KEY_R=19
KEY_T=20
KEY_Y=21
KEY_U=22
KEY_I=23
KEY_O=24
KEY_P=25
KEY_A=30
KEY_S=31
KEY_D=32
KEY_F=33
KEY_G=34
KEY_H=35
KEY_J=36
KEY_K=37
KEY_L=38
KEY_DELETE=111
KEY_Z=44
KEY_X=45
KEY_C=46
KEY_V=47
KEY_B=48
KEY_N=49
KEY_M=50
KEY_DOT=52

KEY_PAGEDOWN=104
KEY_PAGEUP=109
KEY_RIGHTMETA=126
KEY_SYM=KEY_RIGHTMETA

KEY_F20=190
KEY_F21=191
KEY_F22=192
KEY_F23=193
KEY_F24=194
KEY_BACKSPACE=14

KEY_FONT=KEY_F20
KEY_OK=KEY_F24
KEY_DEL=KEY_BACKSPACE

KEY_ENTER=28
KEY_UP=103
KEY_LEFT=105
KEY_RIGHT=106
KEY_DOWN=108
KEY_MENU=139

KEY_BACK=158

KEY_USBPLUGIN=44
KEY_USBCONFIG=41
KEY_USBUNCONFIG=42
KEY_USBPLUGOUT=43
KEY_HOME=102

KEY_SCREENSAVERIN=0
KEY_SCREENSAVEROUT=1
KEY_USBPLUGIN=2
KEY_USBPLUGOUT=3
KEY_LINES = [
[('Home', KEY_HOME), ('UsblugIn', KEY_USBPLUGIN), ('UsbPlugOut', KEY_USBPLUGOUT)],
[('ScreenSaverIn', KEY_SCREENSAVERIN), ('ScreenSaverOut', KEY_SCREENSAVEROUT)]
]
 

#define KEY_FONT		KEY_F20
#define KEY_RPAGEUP		KEY_F23
#define KEY_RPAGEDOWN	KEY_F21
#define KEY_OKAY		KEY_F24
#define KEY_DEL			KEY_BACKSPACE

DEVICE_TOUCH_NONE = 0
DEVICE_TOUCH_DOWN = 1
DEVICE_TOUCH_END = 2
DEVICE_TOUCH_UPDATE = 3

class App:
    def book_on_mouse_move(self, event):
        s = struct.pack("iii", DEVICE_TOUCH_UPDATE, int(event.x/SCALE), int(event.y/SCALE))
        os.write(self.touch_fifo, s)
        print "mouse move (%d, %d)" % (int(event.x/SCALE), int(event.y/SCALE))
        pass

    def book_on_lbtndown(self, event):
        s = struct.pack("iii", DEVICE_TOUCH_DOWN, int(event.x/SCALE), int(event.y/SCALE))
        os.write(self.touch_fifo, s)
        print "mouse left button down (%d, %d)" % (int(event.x/SCALE), int(event.y/SCALE))
        pass

    def book_on_lbtnup(self, event):
        s = struct.pack("iii", DEVICE_TOUCH_END, int(event.x/SCALE), int(event.y/SCALE))
        os.write(self.touch_fifo, s)
        print "mouse left button up (%d, %d)" % (int(event.x/SCALE), int(event.y/SCALE))
        pass

    def book_on_key(self, event):
        print 'book_on_key %r' % event.keycode
        print type(event.keysym)
        print ord(event.keycode[0]), ord(event.keycode[1])
        print len(event.keycode)        
        pass
        
    def usb_send_key(self, x):
        s = 'killall -%d KindleApp' % x
        os.system(s)
            
    def usb_factory(self, x):
        print 'usb_factory'
        return lambda:self.usb_send_key(x)
        
    def sendkey_factory(self, x):
        return lambda:self.sendkey(x)

    def sendkey(self, x):
        print 'send %s' % chr(x)
        os.write(self.key_fifo, chr(x))

    def __init__(self,width,height):
        self.width = width
        self.height = height
        self.key_fifo=os.open(KindleKeyPath, os.O_WRONLY)
        self.touch_fifo=os.open(KindleTouchPath, os.O_WRONLY)
        self.file_opt = options = {}
        options['defaultextension'] = '.png'
        options['initialdir']='~/Pictures'

        self.root = Tkinter.Tk()
        self.frame = Tkinter.Frame(self.root)
        self.frame.pack()
        self.book = Tkinter.Label(self.frame, image=None, text="touch", width = int(self.width * SCALE), height=int(self.height * SCALE))
        self.book.image = None  
        self.book.bind("<Key>", self.book_on_key)
        self.book.bind("<Button-1>", self.book_on_lbtndown)
        self.book.bind("<ButtonRelease-1>", self.book_on_lbtnup)
        self.book.bind("<B1-Motion>", self.book_on_mouse_move)
        self.book.pack(side=Tkinter.LEFT)
        self.keys = []
        
        i = 0
        for line in KEY_LINES:
            frame = Tkinter.Frame(self.frame)
            for k in line:
                keyBtn = Tkinter.Button(frame, text=k[0], command=self.sendkey_factory(k[1]))
                keyBtn.pack(side=Tkinter.LEFT)
            frame.pack(side=Tkinter.TOP)
            self.keys.append(frame)
            i += 1
        frame = Tkinter.Frame(self.frame);
        btn = Tkinter.Button(frame, text="PrintScreen", command=self.print_screen)
        btn.pack(side=Tkinter.LEFT)
        frame.pack(side=Tkinter.TOP)
        self.keys.append(frame)
        self.update_img()
        
    def run(self):
        self.root.mainloop()
    def update_img(self):
        with open(KindleImgPath, "r+b") as f:
            kindleImg = mmap.mmap(f.fileno(), 0)
            s = "".join(map(lambda x: chr(255 - ord(x[0])), kindleImg))
            img = Image.fromstring("L", (self.width,self.height), s[0:]) #,"L;I", 600, 1)
            img = img.resize((int(img.size[0] * SCALE), int(img.size[1] * SCALE)))
            imgTk = ImageTk.PhotoImage(img)
            self.book.configure(image=imgTk)
            self.book.image = imgTk;
            self.curImg = img;
        self.root.after(200, self.update_img)
    

    def print_screen(self):
        filename = tkFileDialog.asksaveasfilename(**self.file_opt)
        if filename:
            print dir(self.book.image)
            self.curImg.save(filename)

    def say_hi(self):
        print "hi there, everyone"
 
opts,args = getopt.getopt(sys.argv[1:], "d:w:h:", ["width", "height"])
print opts
print args
width = -1
height = -1
deviceType = ""
for o, a in opts:
    if o in ("-w", "--width"):
        width = int(a)
    elif o in ("-h", "--height"):
        height = int(a)
    elif o in ("-d", "--devicetype"):
        deviceType = a;
        
if width == -1:
    width = 600
if height == -1:
    height = 800
if deviceType=="k3" or deviceType=="k4" or deviceType=="kt":
    width = 600
    height = 800
elif deviceType=="kp":
    width = 758
    height = 1024
print width 
print height
app = App(width,height)
app.run()



    
