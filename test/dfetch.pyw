#! /usr/bin/env python3

from tkinter import * #gui
from idlelib import ToolTip #hint bubbles
from serial import Serial #serial communication
from threading import Thread #threading
from math import log #temperature computation
from time import sleep #wait for recovery (when comm error)

#Port settings
PortName    = 'COM6'
PortSpeed   = 9600
PortTimeout = 0.25
StrQuestion = '?'

MAX_RX_ERRORS = 3

class thrDataFetch(Thread):
    ''' analog sensor data fetching thread '''

    def __init__(self,port,speed,timeout,qstr,outf,stopf):
        Thread.__init__(self)
        #store input args and functions
        self.portName=port
        self.portSpeed=speed
        self.portTimeout=timeout
        self.requestStr=qstr
        self.outFunction=outf
        #stop flag
        self.stopFlag = False
        self.sendFlag = False
        self.ToSend = 0x0000;
        #onstop function
        self.stopFunction=stopf;
        #comm error count
        self.txErrCount = 0
        self.rxErrCount = 0

    def run(self):
        #with Serial(self.portName,self.portSpeed,timeout=self.portTimeout) as port:
        try:
            with Serial(self.portName,self.portSpeed,bytesize=8, parity='N', stopbits=1, timeout=self.portTimeout, xonxoff=0, rtscts=0) as port:
                stopreason='stopped normally'
                while (self.stopFlag==False):
                    try:
                        port.write(self.requestStr.encode('ascii'))
                        answ = port.readlines()
                        results = answ[-1].strip().decode('ascii')
                        results = results.split(',')
                        self.outFunction(results)
                        #print(results)
                        self.rxErrCount=0
                    except:
                        self.rxErrCount+=1
                    #test comm errors
                    if self.rxErrCount>=MAX_RX_ERRORS:
                        stopreason='data receiving errors'
                        break
                    
                port.close()
                self.stopFunction(stopreason)
        except:
            self.stopFunction('can\'t open serial port')

    def send(self,data):
        try:
            self.ToSend = int(data)
            self.sendFlag = True
        except:
            pass

    def stopReg(self):
        self.stopFlag = True


#application class
class runapp_gui(Frame):
    ''' analog sensor test gui '''

    def __init__(self,master=None):
        self.root = Tk()
        try:
            self.root.iconbitmap(default='img/launchpad.ico')
        except:
            pass
        Frame.__init__(self,master)
        self.portName = PortName
        self.setTitle()
        self.createWidgets()

    def setTitle(self):
        'set window title'
        self.root.title('MSP430 SHT11 ({})'.format(self.portName))

    def createWidgets(self):
        ''' creating form widgets (artwork) '''

        #toolbar
        acol=0
        self.frmToolBar = Frame(self.root,borderwidth=3)
        self.frmToolBar.pack(fill=X, padx=2, pady=2)
        #left side buttons
        self.frmLeftBtns = Frame(self.frmToolBar)
        self.frmLeftBtns.pack(side=LEFT)
        self.boolConnected = False
        self.imgConnect = PhotoImage(file='img/connect.gif')
        self.btnConnect = Button(self.frmLeftBtns,image=self.imgConnect,
                                 command=self.connectClick)
        self.btnConnect.grid(row=0, column=acol)
        ToolTip.ToolTip(self.btnConnect,'Connect/Disconnect')
        acol += 1
        self.imgSettings = PhotoImage(file='img/settings.gif')
        self.btnSettings = Button(self.frmLeftBtns,image=self.imgSettings,
                                  command=self.configClick)#,state=DISABLED)
        self.btnSettings.grid(row=0, column=acol)
        ToolTip.ToolTip(self.btnSettings,'Settings')

        #info (in toolbar)
        acol += 1
        self.lblInfo = Label(self.frmToolBar,text='Disconnected',anchor=CENTER)
        self.lblInfo.pack(fill=BOTH,expand=1)
 
        # separator
        self.separator = Frame(height=2, bd=1, relief=SUNKEN)
        self.separator.pack(fill=X, padx=5, pady=5)

        # temperature and humidity frame
        self.frmTempHumi = Frame(self.root,borderwidth=3)
        self.frmTempHumi.pack(side=TOP,fill=X,expand=1)
        
        # temperature
        self.lblTemp = Label(self.frmTempHumi,text='Temperature :',
                             anchor=E)
        self.lblTemp.grid(row=0, column=0, sticky=E)
        self.strTemp = StringVar()
        self.entTemp = Entry(self.frmTempHumi,
                             textvariable=self.strTemp,
                             font='Courier',
                             width=5,
                             justify=RIGHT,
                             state=DISABLED)
        self.entTemp.grid(row=0, column=1)
        self.lblTempstr = Label(self.frmTempHumi,text='°C',
                                anchor=W,width=5)
        self.lblTempstr.grid(row=0, column=2, sticky=W)

        # humidity
        self.lblHumi = Label(self.frmTempHumi,text='Humidity :',
                             anchor=E)
        self.lblHumi.grid(row=0, column=3, sticky=E)
        self.strHumi = StringVar()
        self.entHumi = Entry(self.frmTempHumi,
                             textvariable=self.strHumi,
                             font='Courier',
                             width=5,
                             justify=RIGHT,
                             state=DISABLED)
        self.entHumi.grid(row=0, column=4)
        self.lblHumistr = Label(self.frmTempHumi,text='%',
                                anchor=W)
        self.lblHumistr.grid(row=0, column=5, sticky=W)

        # separator
        self.separator = Frame(height=2, bd=1, relief=SUNKEN)
        self.separator.pack(fill=X, padx=5, pady=5)

    def configClick(self):
        ''' on config button click '''

        from serselect import serscan
        self.configWindow = Toplevel()
        self.configFrame = Frame(self.configWindow)
        self.configFrame.pack()
        ports = serscan.scan()
        self.strPort = StringVar()
        if len(ports)>0:
            for p in ports:
                b = Radiobutton(self.configFrame, text=p[1], variable=self.strPort, value=p[1])
                b.pack(side=TOP, expand=YES, pady=2, anchor='w')
            self.btnPortSelect = Button(self.configFrame, text='OK', command=self.portSelectClick)
            self.btnPortSelect.pack(fill=X,expand=1,pady=2)
        else:
            l = Label(self.configFrame,text='No port found !')
            l.pack()

    def portSelectClick(self):
        'ok button click (in serial select radio group)'

        self.configWindow.destroy()
        if self.strPort.get()!='':
            self.portName=self.strPort.get()
        self.setTitle()
        

    def connectClick(self):
        ''' on connect button click
        it tests if connect thread is running and decide if it is wanted to stop
        or start it (and it is doing some button visualisation too) '''
        
        if self.boolConnected:
            #disconnect
            self.snsThread.stopReg()
            self.btnConnect.config(relief=RAISED,state=DISABLED)
        else:
            #connect
            self.snsThread = thrDataFetch(self.portName,PortSpeed,PortTimeout,StrQuestion,
                                          self.showData,self.onStopComm)
            self.snsThread.start()
            self.boolConnected = True
            self.btnConnect.config(relief=RIDGE,state=DISABLED)
            self.btnSettings.config(state=DISABLED)

    def showData(self,result):
        '''this is called by serial thread when some data fetched from sensor
        - function gets fetched data in list form '''

        try:
            if len(result)>=2:
                #show result
                try:
                    r0 = int(result[0],10)/10
                    if r0>=800:
                        r0=-(r0-800)
                    self.strTemp.set('{}'.format(r0))
                    r1 = int(result[1],10)/10
                    self.strHumi.set('{}'.format(r1))
                except:
                    pass
                '''self.strVoltADC.set('0x{}'.format(result[0]))
                self.strTempADC.set('0x{}'.format(result[1]))'''
                #show info (status)
                self.lblInfo.config(text='Connected')
                self.btnConnect.config(state=NORMAL)
        except:
            self.lblInfo.config(text='Calcutation error')

    def onStopComm(self,reason):
        '''this is called by serial thread when it stops
        - function gets stop reason in text form '''

        sleep(0.1) #time to recovery in case of error
        
        self.boolConnected = False
        self.btnConnect.config(relief=RAISED,state=NORMAL)

        self.lblInfo.config(text='Connection {}'.format(reason))

        self.btnSettings.config(state=NORMAL)


app = runapp_gui()
app.mainloop()

