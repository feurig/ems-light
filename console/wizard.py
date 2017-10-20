import wx
import wx.wizard
import images
import subprocess
import  wx.lib.newevent
import threading

Progress2Event, EVT_PROGRESS2 = wx.lib.newevent.NewEvent()
Progress3Event, EVT_PROGRESS3 = wx.lib.newevent.NewEvent()
Progress4Event, EVT_PROGRESS4 = wx.lib.newevent.NewEvent()

class TitledPage(wx.wizard.WizardPageSimple):
    def __init__(self, parent, title, stage):
        wx.wizard.WizardPageSimple.__init__(self, parent)
        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.parent=parent
        self.SetSizer(self.sizer)
        self.title=title
        self.stage=stage
        
        self.titleText = wx.StaticText(self, -1, title)
        self.titleText.SetFont(
                               wx.Font(18, wx.SWISS, wx.NORMAL, wx.BOLD))
        self.sizer.Add(self.titleText, 0,
                       wx.ALIGN_CENTRE | wx.ALL, 5)
        self.sizer.Add(wx.StaticLine(self, -1), 0,
                       wx.EXPAND | wx.ALL, 5)


class backupThread(threading.Thread):
    def __init__(self, parent, portname):
        """
            @param parent: The gui object that should recieve the value
            @param value: value to 'calculate' to
            """
        threading.Thread.__init__(self)
        self.sender = parent
        self.portname = portname
    
    def run(self):
        percent=0
        p=subprocess.Popen('avrdude -v -P'+self.portname+' -cstk500v2 -pm2560 -Uflash:r:test.hex:i' ,shell=True,stderr=subprocess.PIPE, stdout=subprocess.PIPE)
        while (p.stderr.readline()):
            percent = percent + 1
            evt = Progress2Event(progress=percent/2,result=-1)
            wx.PostEvent(self.sender, evt)
        
        evt = Progress2Event(progress=100,result=p.poll())
        wx.PostEvent(self.sender, evt)


class updateThread(threading.Thread):
    def __init__(self, parent, portname):
        """
            @param parent: The gui object that should recieve the value
            @param value: value to 'calculate' to
            """
        threading.Thread.__init__(self)
        self.sender = parent
        self.portname = portname
    
    def run(self):
        percent=0
        p=subprocess.Popen('make install' ,cwd='../../csource/EMS/' ,shell=True,stderr=subprocess.PIPE, stdout=subprocess.PIPE)
        while (p.stderr.readline()):
            percent = percent + 1
            evt = Progress3Event(progress=percent/3,result=-1)
            wx.PostEvent(self.sender, evt)
        
        evt = Progress3Event(progress=100,result=p.poll())
        wx.PostEvent(self.sender, evt)




class EMSUpdateWizard(wx.wizard.Wizard):
    def __init__(self, parent,id,title,bitmap):
        wx.wizard.Wizard.__init__(self,parent,id,title,bitmap)
        #self.device=None
        self.portname=None
        self.deviceInUse=False
        self.oldSWV=None
        self.ssn=None
        self.page1 = TitledPage(self, "Connecting to EMS Device",1)
        self.page2 = TitledPage(self, "Upgrading Firmware",2)
        self.page3 = TitledPage(self, "Backing Up Log Files",3)
        self.page4 = TitledPage(self, "Summary",4)
        
        self.backButton = self.FindWindowById(wx.ID_BACKWARD)
        self.forwardButton = self.FindWindowById(wx.ID_FORWARD)
        self.backButton.Disable()
        self.forwardButton.Disable()
        
        self.blurb=[1,2,3,4,5]
        self.blurb[1] = wx.StaticText(self.page1, -1, "Please Connect the EMS")
        self.page1.sizer.Add(self.blurb[1], 1, wx.ALIGN_CENTRE | wx.ALL, 5)
        self.emsSWV=[1,2,3,4,5]
        self.emsSWV[1]=wx.StaticText(self.page1, -1,
                                     "EMS Software Version: <..Searching..>")
        self.page1.sizer.Add(self.emsSWV[1])
        self.blurb[2] = wx.StaticText(self.page2, -1,
                                      "backing up firmware")
        self.page2.sizer.Add(self.blurb[2])
        self.statusLine2=wx.StaticText(self.page2, -1, "...")
        self.page2.sizer.Add(self.statusLine2)
        self.progress2 = wx.Gauge(self.page2, -1, 100, wx.Point(-1, -1), (250, 25))
        self.page2.sizer.Add(self.progress2,0,wx.EXPAND | wx.HORIZONTAL)
        self.statusLine3=wx.StaticText(self.page2, -1, "")
        self.page2.sizer.Add(self.statusLine3)
        self.progress3 = wx.Gauge(self.page2, -1, 100, wx.Point(-1, -1), (250, 25))
        self.page2.sizer.Add(self.progress3,0,wx.EXPAND | wx.HORIZONTAL)
        self.page2.sizer.Add((25, 50),1,wx.EXPAND | wx.HORIZONTAL)
        self.emsSWV[2]=wx.StaticText(self.page2, -1,
                                     "EMS Software Version: <..Changing..>")
        self.page2.sizer.Add(self.emsSWV[2])
        
        self.blurb[3] = wx.StaticText(self.page3, -1, "backing up logger")
        self.page3.sizer.Add(self.blurb[3])
        self.page2.sizer.Add((25, 50),1,wx.EXPAND | wx.HORIZONTAL)
        self.emsSWV[3]=wx.StaticText(self.page3, -1,
                                     "EMS Software Version: <..Searching..>")
        self.page3.sizer.Add(self.emsSWV[3])
        
        self.blurb[4] = wx.StaticText(self.page4, -1, "Finishing up")
        self.page4.sizer.Add(self.blurb[4])
        
        self.oldSWVField = wx.StaticText(self.page4, -1, ".")
        self.page4.sizer.Add(self.oldSWVField)
        self.page2.sizer.Add((25, 50),1,wx.EXPAND | wx.HORIZONTAL)
        self.emsSWV[4]=wx.StaticText(self.page4, -1,
                                     "EMS Software Version: <..Searching..>")
        self.page4.sizer.Add(self.emsSWV[4])
        
        
        wx.wizard.WizardPageSimple_Chain(self.page1, self.page2)
        wx.wizard.WizardPageSimple_Chain(self.page2, self.page3)
        wx.wizard.WizardPageSimple_Chain(self.page3, self.page4)
        
        self.Bind(wx.wizard.EVT_WIZARD_PAGE_CHANGED, self.on_page_changed)
        self.Bind(wx.wizard.EVT_WIZARD_PAGE_CHANGING, self.on_page_changing)
        self.Bind(wx.wizard.EVT_WIZARD_CANCEL, self.on_cancel)
        
        self.Bind(wx.EVT_IDLE, self.OnWizardIdle)
        
        self.Bind(EVT_PROGRESS2, self.onUpdateProgress2)
        self.Bind(EVT_PROGRESS3, self.onUpdateProgress3)
        
        self.idleTimer = wx.Timer(self,-1)
        self.Bind(wx.EVT_TIMER, self.idleWatchdog, self.idleTimer)
        self.idleTimer.Start(2000)
    
    #    def OnInit(self):
    #self.device=None
    #self.Bind(wx.wizard.EVT_WIZARD_FINISHED, self.on_finished)
    
    def idleWatchdog(self,evt):
        wx.WakeUpIdle()
    
    
    
    def OnWizardIdle(self, event):
        #print "we have an idle event"
        if not self.deviceInUse :
            if self.device.attached:
                #self.statusBar1.SetStatusText("Attached:"+str(self.device.device_ssn_string),1)
                line=self.device.ser.readline(100).rstrip('\r\n')
                line=line.encode('ascii', 'ignore')
                self.device.dispatch(line)
                
                if self.device.device_ssn_string is None:
                    self.device.attached=False
                    return
                self.portname=self.device.portname
                if self.stage == 1 :
                    self.blurb[self.stage].SetLabel('Found EMS #'+self.device.device_ssn_string)
                    self.oldSWV=self.device.device_swv_string
                    self.SSN=self.device.device_ssn_string
                self.emsSWV[self.stage].SetLabel('Current Firmware: '+self.device.device_swv_string)
                self.forwardButton.Enable()
    
            else:
                #self.statusBar1.SetStatusText("No device Connected!",1)
                self.device.attached=False
                #self.device.ser.close()
                #self.statusBar1.SetStatusText("...Searching...",1)
                self.device.open(self.device.portname,9600)
        #self.OnReconnect()
    
        event.Skip()

    
    def on_page_changed(self, evt):
        '''Executed after the page has changed.'''
        if evt.GetDirection():  dir = "forward"
        else:                   dir = "backward"
        self.forwardButton = self.FindWindowById(wx.ID_FORWARD)
        self.forwardButton.Disable()
        self.backButton = self.FindWindowById(wx.ID_BACKWARD)
        self.backButton.Disable()
        
        page = evt.GetPage()
        self.stage=page.stage
        
        if page.stage == 1: self.doStage1()
        if page.stage == 2: self.doStage2()
        if page.stage == 3: self.doStage3()
        if page.stage == 4: self.doStage4()
    
    #print "page_changed: %s, %s, %d\n" % (dir, page.__class__, page.stage)
    
    def on_page_changing(self, evt):
        self.forwardButton.Disable()
        '''Executed before the page changes, so we might veto it.'''
        if evt.GetDirection():  dir = "forward"
        else:                   dir = "backward"
        page = evt.GetPage()
        print "page_changing: %s, %s, %d\n" % (dir, page.__class__, page.stage)
    
    def on_cancel(self, evt):
        from pam import authenticate
        '''Cancel button has been pressed.  Clean up and exit without continuing.'''
        page = evt.GetPage()
        print "on_cancel: %s\n" % page.__class__,page.title
        dialog=wx.TextEntryDialog(None,"Enter Password to Log In",defaultValue='',
                                  style=wx.OK|wx.CANCEL|wx.TE_PASSWORD)
        if dialog.ShowModal() == wx.ID_OK:
            if authenticate('update',dialog.GetValue()) is False:
                wx.MessageBox("The update system will shut down now", "Shutting Down")
                subprocess.Popen('sudo /sbin/shutdown -h now', shell=True)
        else:
            evt.Veto()
        dialog.Destroy()

    
    def doStage1(self):
        self.deviceInUse=False
        if (self.device.attached):
            self.portname=self.device.portname
            self.blurb[1].SetLabel('Found EMS #'+self.device.device_ssn_string)
            self.emsSWV[1].SetLabel('Current Firmware: '+self.device.device_swv_string)
            self.oldSWV=self.device_swv_string
            self.forwardButton.Enable()
        print "doing stage1 now"
    
    
    def onUpdateProgress2(self,evt):
        if(evt.result==-1):
            self.statusLine2.SetLabel("..running..")
            self.progress2.SetValue(evt.progress)
        if(evt.result==0):
            self.statusLine2.SetLabel("Success")
            self.deviceInUse=False
            self.progress2.SetValue(100)
            #self.forwardButton.Enable()
            self.doStage2a()
        if(evt.result==1):
            self.deviceInUse=False
            self.statusLine2.SetLabel("FAIL!")
            self.progress2.SetValue(0)
    
    def onUpdateProgress3(self,evt):
        if(evt.result==-1):
            self.statusLine3.SetLabel("..running..")
            self.progress3.SetValue(evt.progress)
        if(evt.result==0):
            self.statusLine3.SetLabel("Success")
            self.deviceInUse=False
            self.progress3.SetValue(100)
            self.forwardButton.Enable()
        #self.doStage2a()
        if(evt.result==1):
            self.deviceInUse=False
            self.statusLine3.SetLabel("FAIL!")
            self.progress3.SetValue(0)
    
    
    def doStage2(self):
        self.blurb[2].SetLabel("Saving Old Firmware")
        self.deviceInUse=True
        self.device.attached=False
        percent = 0
        self.progress2.SetValue(percent)
        self.device.ser.close()
        self.forwardButton.Disable()
        worker = backupThread(self, self.portname)
        worker.start()
        print "doing stage2 now"
    
    def doStage2a(self):
        self.blurb[2].SetLabel("Loading New Firmware")
        self.deviceInUse=True
        self.device.attached=False
        percent = 0
        self.progress3.SetValue(percent)
        self.device.ser.close()
        self.forwardButton.Disable()
        worker = updateThread(self, self.portname)
        worker.start()
        print "doing stage2a now"
    
    def doStage3(self):
        print "doing stage3 now"
    
    def doStage4(self):
        if self.oldSWV is not None:
            self.oldSWVField.SetLabel("Old Firmware:"+self.oldSWV)
        else:
            self.oldSWVField.SetLabel("Old Firmware:<Unknown>")
        print "doing stage4 now"


def doTheWizard(ems):
    wizard = emsUpdateWizard(None, -1, "EMS Firmware Updater",images.ems.GetBitmap())
    wizard.device=ems
    ems.sendCMD("SWV?\r\nHWV?")
    wizard.FitToPage(wizard.page1)
    
    if wizard.RunWizard(wizard.page1):
        wx.MessageBox("Update Complete! system will shut down now", "Shutting Down")
        subprocess.Popen('sudo /sbin/shutdown -h now',shell=True)
        print "Success"

    self.idleTimer.Stop()



def checkIfDone(done):
    btn = theWizard.FindWindowById(wx.ID_FORWARD)
    if btn:
        btn.Disable()
    
    wizard.Destroy()

class Background(wx.Frame):
    """class Panel1 creates a panel with an image on it, inherits wx.Panel"""
    def __init__(self, parent, id, ems):
        # create the panel
        wx.Frame.__init__(self, parent, id, 'EMS Firmware Updater')
        self.ems=ems
        panel = wx.Panel(self)
        try:
            # pick an image file you have in the working folder
            # you can load .jpg  .png  .bmp  or .gif files
            image_file = 'background.jpg'
            bmp1 = wx.Image(image_file, wx.BITMAP_TYPE_ANY).ConvertToBitmap()
            # image's upper left corner anchors at panel coordinates (0, 0)
            self.bitmap1 = wx.StaticBitmap(panel, -1, bmp1, (0, 0))
        # show some image details
        
        except IOError:
            print "Image file %s not found" % imageFile
            raise SystemExit
        self.Show()



# button goes on the image --> self.bitmap1 is the parent
#self.button1 = wx.Button(self.bitmap1, id=-1, label='Button1', pos=(8, 8))

if __name__ == "__main__":
    from EMS import EMS
    app = wx.App()
    frame = Background(None, -1, EMS())
    #frame.Show()
    frame.ShowFullScreen(True, style= wx.DEFAULT_FRAME_STYLE |wx.NO_FULL_REPAINT_ON_RESIZE | wx.FULLSCREEN_ALL
                         |wx.FULLSCREEN_NOTOOLBAR | wx.FULLSCREEN_NOBORDER | wx.FULLSCREEN_NOCAPTION
                         | wx.FULLSCREEN_NOCAPTION)
    doTheWizard(frame.ems)
    frame.Close()
#app.MainLoop()


"""
    def find_logging_device_name():
    
    plat_sys = platform.system()
    plat_bits = platform.architecture()[0]
    if plat_sys == 'Linux':
    prefix='/media/'
    elif plat_sys == 'Darwin':
    prefix=/Volumes/'
    possible_paths = list()
    for (path, dirs, files) in os.walk(file_prefix):
    mypath=path
    mydir=dirs
    for x in files:
    if 'LOGCON.txt' in x:
    return mypath
    """
