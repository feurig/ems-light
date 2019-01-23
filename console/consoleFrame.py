"""
*----------------------------------------------------------consoleFrame.py
* Author: Donald Delmar Davis, Suspect Devices
* Liscence: "Simplified BSD License"
*
* Copyright (c) 2016, Donald Delmar Davis, Suspect Devices
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in thedocumentation
* and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
**
"""

import wx
import datetime
import os
import subprocess

EMS_build_dir=os.path.dirname(os.path.realpath(__file__)) + os.sep + '../Arduino/ems/'
main_ems_dir=os.path.dirname(os.path.realpath(__file__)) + os.sep + '../'

def create(parent):
    return EMSConsole(parent)


class EMSConsole(wx.Frame):



    def _init_sizers(self):
        self.outSizer=wx.BoxSizer(orient=wx.VERTICAL)
        self.boxSizer1 = wx.BoxSizer(orient=wx.HORIZONTAL)
        self.consoleSizer=wx.BoxSizer(orient=wx.VERTICAL)
        self.consoleSizer.Add(self.emsOutput, 2, border=0,flag=wx.EXPAND|wx.ALL)
        self.consoleSizer.Add(self.emsCommand, 0, border=1,
                                 flag=wx.EXPAND | wx.ALL | wx.ALIGN_LEFT)
                                 
        self.runSizer = wx.BoxSizer(orient=wx.VERTICAL)
        self.runSizer1=wx.BoxSizer(orient=wx.VERTICAL)
        self.runSizer1.Add(wx.StaticText(label=u'SEC. REMAINING',
                                               parent=self,
                                               style = wx.ALIGN_CENTER),
                                 0, border=10,
                                 flag=wx.CENTER)
        self.runSizer1.Add(self.TRM,1, border=10,
                                 flag=wx.EXPAND|wx.ALL)
        #self.runSizer1.AddSpacer((30,20),1)
                                 
        self.runSizer.Add(self.runSizer1,1, border=0, flag=wx.CENTER)
        self.runSizer.Add(self.RUN,0, border=0, flag=wx.CENTER)
        self.runSizer.Add(self.TMS, 1, border=0,flag=wx.EXPAND)
        #self.runSizer.AddSpacer((20,20))
        
        self.pumpSizer = wx.BoxSizer(orient=wx.VERTICAL)
        self.pumpSizer.Add(self.MUP,1, border=0, flag=wx.CENTER|wx.ALIGN_CENTER)
        self.pumpSizer2 = wx.BoxSizer(orient=wx.VERTICAL)
        self.pumpSizer2.Add(wx.StaticText(label=u'PLATE PRESSURE',
                                               parent=self),
                                 0,border=0,flag=wx.CENTER)
        self.pumpSizer2.Add(self.PS1,1, border=10,
                                  flag=wx.EXPAND|wx.ALL)
                                  
        self.pumpSizer2.Add(wx.StaticText(label=u'(Approximate)',parent=self,
                                                style = wx.ALIGN_CENTER
                                                ),
                                  
                                  0, border=0,flag=wx.CENTER)
                                  
        self.pumpSizer.Add(self.pumpSizer2,1, border=0, flag=wx.CENTER)
        self.pumpSizer.Add(self.MDN,1, border=0, flag=wx.CENTER)
        self.pumpSizer1=wx.BoxSizer(orient=wx.HORIZONTAL)
        self.pumpSizer1.Add(self.SOL, 0, border=0, flag=wx.ALIGN_CENTER)
        self.pumpSizer1.Add(self.PMS, 0, border=0, flag=wx.ALIGN_CENTER)
        self.pumpSizer.Add(self.pumpSizer1,0, flag=wx.CENTER)

        self.tempSizer = wx.BoxSizer(orient=wx.VERTICAL)
        self.tempSizer2 = wx.BoxSizer(orient=wx.HORIZONTAL)
        self.tempSizer2.Add(self.TS1, 0, border=5,flag=wx.EXPAND|wx.BOTTOM)
        self.tempSizer2.Add(self.TS2, 0, border=5,flag=wx.EXPAND|wx.BOTTOM)
        self.tempSizer2.Add(self.TMP, 1,
                                  border=5,
                                  flag=wx.CENTER)
        self.tempSizer2.Add(0,30,10)
        self.tempSizer.Add(self.tempSizer2, 1, border=1, flag=wx.CENTER)
        self.tempSizer.Add(self.HSW, 0, border=0, flag=wx.CENTER)


        self.boxSizer1.Add(self.consoleSizer, 1, border=0, flag=wx.CENTER)
        self.boxSizer1.Add(self.pumpSizer, 1, border=0, flag=wx.CENTER)
        self.boxSizer1.Add(self.runSizer, 1, border=0, flag=wx.CENTER)
        self.boxSizer1.Add(self.tempSizer, 1, border=0, flag=wx.CENTER)
        self.outSizer.Add(self.boxSizer1, 1, border=0, flag=wx.CENTER)
        self.outSizer.Add(self.statusBar1, 0, border=5, flag=wx.BOTTOM)
        
        self.SetSizer(self.outSizer)

    def _init_ctrls(self, prnt):
        wx.Frame.__init__(self, name=u'EMSConsole',
              parent=prnt, size=(800, 480),
                          #style=wx.CAPTION,
                          style=wx.FRAME_NO_TASKBAR|wx.SIMPLE_BORDER,
                          #style=wx.MINIMIZE_BOX | wx.MAXIMIZE_BOX
                          #| wx.RESIZE_BORDER | wx.CAPTION
                          #| wx.CLIP_CHILDREN | wx.CLOSE_BOX|wx.FRAME_NO_TASKBAR,
                          title=u'Console')
        self.SetClientSize((700, 420))
        self.Bind(wx.EVT_IDLE, self.OnEMSConsoleIdle)

        self.statusBar1=self.CreateStatusBar()
        self.statusBar1.SetFieldsCount(4)
        self.statusBar1.SetStatusWidths([-1,-1,-1,-1])
        
        self.emsCommand = wx.TextCtrl(
              name=u'emsCommand', parent=self,
              size=(200, 22), style=wx.TE_PROCESS_ENTER,
              value=u'')
              
        self.emsCommand.SetInsertionPoint(1)
        self.emsCommand.Bind(wx.EVT_TEXT_ENTER, self.OnEMSCommandTextEnter)
        self.emsCommand.Bind(wx.EVT_KILL_FOCUS, self.OnEMSCommandKillFocus)

        self.emsOutput = wx.TextCtrl(name=u'emsOutput', parent=self,
                                     size=(200,300), style=wx.TE_MULTILINE, value=u'...')
        self.emsOutput.SetEditable(False)
        self.emsOutput.SetAutoLayout(True)
        self.emsOutput.Enable(False)
        
        self.bigFont=wx.Font(24, wx.ROMAN, wx.BOLD, wx.NORMAL)
        
        self.PS1 = wx.StaticText(name=u'PS1',
                                label=u'0000',
                                parent=self,
                                #size=(30, 20),
                                style = wx.ALIGN_CENTER)
        self.PS1.SetFont(self.bigFont)
                                 
                                 

        self.TMS = wx.Slider( maxValue=300, minValue=0,
                             size=(120, 30),
                             name=u'TMS', parent=self,
                             style=wx.SL_HORIZONTAL|wx.SL_LABELS,
                             value=0)
        self.TMS.Bind(wx.EVT_COMMAND_SCROLL_THUMBRELEASE,
              self.OnTMSCommandScrollThumbrelease)
    
        self.TMS.Bind(wx.EVT_SCROLL_THUMBTRACK, self.OnTMSCommandScrollThumb)
        

        self.playb = wx.Bitmap(os.path.dirname(os.path.realpath(__file__))
                       + os.sep +"play.png",
                       wx.BITMAP_TYPE_ANY)
    
        self.pauseb = wx.Bitmap(os.path.dirname(os.path.realpath(__file__))
                           + os.sep +"pause.png",
                           wx.BITMAP_TYPE_ANY)
        
        self.RUN = wx.BitmapButton(parent=self, id=wx.ID_ANY, bitmap=self.playb,
                                      size=(self.playb.GetWidth(),
                                            self.playb.GetHeight()),
                                      style=wx.NO_BORDER|wx.BU_EXACTFIT)
                             
                             
        self.RUN.Bind(wx.EVT_BUTTON, self.OnRUNButton)
    
        self.TRM = wx.StaticText(name=u'TRM',
                                 label=u'0000',
                                 parent=self,
                                 style = wx.ALIGN_CENTER)

        self.TRM.SetFont(self.bigFont)
                                     
        self.TS1 = wx.Gauge( name=u'TS1',
              parent=self, pos=wx.DefaultPosition, range=280,
              size=(18, 200), style=wx.GA_VERTICAL)

        self.TS2 = wx.Gauge( name=u'TS2',
              parent=self, pos=wx.DefaultPosition, range=280,
              size=(18, 200), style=wx.GA_VERTICAL)

        self.TMP = wx.Slider( maxValue=280, minValue=0, name=u'TMP', parent=self,
                             size=(80, 200),
                             style=wx.SL_VERTICAL
                             |wx.SL_AUTOTICKS
                             |wx.SL_LABELS
                             |wx.SL_LEFT
                             |wx.SL_INVERSE,
                             value=0)
        self.TMP.Bind(wx.EVT_COMMAND_SCROLL_THUMBRELEASE,
              self.OnTMPCommandScrollThumbrelease)
        self.TMP.Bind(wx.EVT_SCROLL_THUMBTRACK,
              self.OnTMPCommandScrollThumb)

        self.HSW = wx.CheckBox(label=u'HEAT',
              name=u'HSW', parent=self)

        self.HSW.SetValue(False)
        self.HSW.Bind(wx.EVT_CHECKBOX, self.OnHSWCheckbox)
                                                  
        self.upb = wx.Bitmap(os.path.dirname(os.path.realpath(__file__))
                             + os.sep +"up.png",
                             wx.BITMAP_TYPE_ANY)
        self.MUP = wx.BitmapButton(parent=self, id=wx.ID_ANY,
                                      bitmap=self.upb,
                                      size=(self.upb.GetWidth(),
                                            self.upb.GetHeight()),
                                      style=wx.NO_BORDER|wx.BU_EXACTFIT)
        self.MUP.Bind(wx.EVT_LEFT_DOWN, self.goingUp)
        self.MUP.Bind(wx.EVT_LEFT_UP, self.pumpOff)
       
        self.dnb = wx.Bitmap(os.path.dirname(os.path.realpath(__file__))
                                         + os.sep +"dn.png",
                                         wx.BITMAP_TYPE_ANY)
        self.MDN = wx.BitmapButton(parent=self, id=wx.ID_ANY, bitmap=self.dnb,
                                      size=(self.dnb.GetWidth(),
                                            self.dnb.GetHeight()),
                                      style=wx.NO_BORDER|wx.BU_EXACTFIT)
        self.MDN.Bind(wx.EVT_LEFT_DOWN, self.goingDown)
        self.MDN.Bind(wx.EVT_LEFT_UP, self.pumpOff)

        self.PMS = wx.CheckBox(label=u'PMS', name=u'PMS', parent=self)
        self.PMS.SetValue(False)
        self.PMS.Bind(wx.EVT_CHECKBOX, self.OnPMSCheckbox)

        self.SOL = wx.CheckBox(label=u'SOL', name=u'SOL', parent=self)
        self.SOL.SetValue(False)
        self.SOL.Bind(wx.EVT_CHECKBOX, self.OnSOLCheckbox)

        self._init_sizers()

    def _init_menubar(self,parent):
               #menu stuff --------------------
        menubar = wx.MenuBar()
        fileMenu = wx.Menu()
        utilitiesMenu= wx.Menu()
        wx.ID_LOG=wx.NewId()
        wx.ID_SETTIME=wx.NewId()
        wx.ID_UPLOAD=wx.NewId()
        wx.ID_RECONNECT=wx.NewId()
        wx.ID_GIT=wx.NewId()
        wx.ID_WIZARD=wx.NewId()
        litem = fileMenu.Append(wx.ID_LOG, 'Log\tctrl-l', 'Log output to file')
        qitem = fileMenu.Append(wx.ID_EXIT, 'Quit\tctrl-q', 'Quit application')
        titem = utilitiesMenu.Append(wx.ID_SETTIME, 'Set Time\tctrl-t', 'Set time on device')
        gitem = utilitiesMenu.Append(wx.ID_GIT, 'Git Pull Firmware\tctrl-g', 'Git pull firmware')
        uitem = utilitiesMenu.Append(wx.ID_UPLOAD, 'Upload Firmware\tctrl-u', 'put new firmware on EMS')
        ritem = utilitiesMenu.Append(wx.ID_RECONNECT, 'Reconnect Serial\tctrl-r', 'Reopen Serial device')
        menubar.Append(fileMenu, '&File')
        menubar.Append(utilitiesMenu, '&Utilities')
        self.SetMenuBar(menubar)
        self.Bind(wx.EVT_MENU, self.OnQuit, qitem)
        self.Bind(wx.EVT_MENU, self.OnLog, litem)
        self.Bind(wx.EVT_MENU, self.OnSetTime, titem)
        self.Bind(wx.EVT_MENU, self.OnUpload, uitem)
        self.Bind(wx.EVT_MENU, self.OnReconnect, ritem)
        self.Bind(wx.EVT_MENU, self.OnGit, gitem)

        self.Bind(wx.EVT_IDLE, self.OnEMSConsoleIdle)
        self.Bind(wx.EVT_SHOW, self.OnShow, self)
    
        

    def __init__(self, parent):
        
        self._init_ctrls(parent)
        self._init_menubar(parent)
        self.parent=parent
        self.emsOutputLines=None
        self.tmsIsBeingTracked=False
        self.tmpIsBeingTracked=False
        self.outputIsbeingLogged=False
        self.logFile=None
        self.logFileName=None
        self.emsClockSkew=None
        
        self.serialTimer = wx.Timer(self,-1)
        self.Bind(wx.EVT_TIMER, self.processSerial, self.serialTimer)
        self.serialTimer.Start(2000)
        #device gets attached after we are created....
         #self.device.dispatch_table['LCS']=self.lcs()
    

    def OnQuit(self, event):
        self.serialTimer.Stop()
        if self.outputIsbeingLogged:
            self.logFile.close()
        self.Close(True)
        
    def OnShow(self,event):
        if self.emsOutputLines is None and self.device.attached:
            self.emsOutputLines=self.device.initialContact
            self.emsOutput.Clear()
            for line in self.emsOutputLines:
                self.emsOutput.AppendText(line+'\n')
    

# need to allow for new filename
    def OnLog(self,event):
        if self.outputIsbeingLogged:
            self.logFile.close()
            self.outputIsbeingLogged=False
        else:
            if self.logFileName is None:
                dlg = wx.FileDialog(self, "Choose a log file:")
            else:
                dlg = wx.FileDialog(self, "Choose a log file:",
                                    defaultFile=self.logFileName)

            if dlg.ShowModal() == wx.ID_OK:
                self.logFileName=dlg.GetPath()
                self.logFile=open(self.logFileName,mode='a+')
                self.outputIsbeingLogged=True
            dlg.Destroy()
        event.Skip()
        
    def OnEMSCommandTextEnter(self, event):
        comm=self.emsCommand.GetValue().rstrip('\r\n')
        self.emsCommand.SetValue('')
        self.statusBar1.SetStatusText(">>"+comm,0)
        if self.device.attached:
            self.device.ser.write(str(comm)+'\r\n')
        event.Skip()

# stubb needs feedback for failure

    def OnUpload(self, event):
        self.statusBar1.SetStatusText("...Uploading...",1)
        self.emsOutput.AppendText('\n---------------------- UPLOAD -------------------\n')
        if self.device.ser is not None:
            self.device.ser.close()
        self.device.attached=False
        p = subprocess.Popen(['make', 'install'],
                              stdout=subprocess.PIPE,
                              stderr=subprocess.STDOUT,
                              cwd=EMS_build_dir)
        retval,e = p.communicate()
        self.emsOutput.AppendText(retval)
        if p.poll():
            self.emsOutput.AppendText('\n-------------------- FAIL --------------------\n')
        else:
            self.emsOutput.AppendText('\n------------------- SUCCESS ------------------\n')
                    #os.system('cd ../csource/EMS/ ; make install')
        self.device.open(self.device.portname,9600)
        event.Skip()

# stubb needs feedback for failure
    def OnGit(self, event):
        self.statusBar1.SetStatusText("...Downloading...",1)
        self.emsOutput.AppendText('\n------------------ DOWNLOAD -----------------\n')
        p = subprocess.Popen(['git', 'pull'],
                              stdout=subprocess.PIPE,
                              stderr=subprocess.STDOUT,
                              cwd=main_ems_dir)
        retval,e = p.communicate()
        self.emsOutput.AppendText(retval)
        if p.poll():
            self.emsOutput.AppendText('\n-------------------- FAIL --------------------\n')
        else:
            self.emsOutput.AppendText('\n------------------- SUCCESS ------------------\n')
#        retval=subprocess.check_output('git pull', cwd=EMS_build_dir,
#                                         stderr=subprocess.STDOUT,shell=True)
#        self.emsOutput.AppendText(retval)
#        self.emsOutput.AppendText('\n-------------------------------------------\n')
#        os.system('git pull')
        event.Skip()
 
    def OnReconnect(self, event=None):
        self.device.attached=False
        #self.device.ser.close()
        self.statusBar1.SetStatusText("...Searching...",1)
        self.device.open(self.device.portname,9600)
        if event is not None:
            event.Skip()

    def OnCMDButton(self, event):
        event.Skip()

    def OnTestButton(self, event):
        self.statusBar1.SetStatusText(self.emsCommand.GetValue(),1)
        event.Skip()

    def OnEMSCommandSetFocus(self, event):
        event.Skip()


    def OnEMSCommandKillFocus(self, event):
        #self.OnEnter(self)
        self.statusBar1.SetStatusText(self.emsCommand.GetValue(),0) 
        event.Skip()

    def adjustEMSOutput(self):
        pass
    
    def scaleTempLiteralForGague(self,literal):
        if literal is None: 
            return 0
        f=float(literal)
        i=int(f)
        return min(max(0,i),299)

    def processSerial(self,event=None):
        if self.device.attached:
            try:
                while self.device.ser.inWaiting():
                    line=unicode(self.device.ser.readline(100).rstrip('\r\n'),errors='ignore')
                    line=line.encode('ascii','replace')
                    if self.emsOutputLines is None:
                        self.emsOutputLines=[]
                    self.emsOutputLines.append(line)
                    if len(self.emsOutputLines) > 1000:
                        self.emsOutputLines=self.emsOutputLines[100:]
                    if self.emsOutput.GetLastPosition()>10000:
                        self.emsOutput.Replace(0,1000,'...\r\n')
                    self.emsOutput.AppendText(line+'\n')
                    if self.outputIsbeingLogged:
                        self.logFile.write(line+'\r\n')
                    self.device.dispatch(line)
            except IOError as e:
                self.device.attached=False
        if event is not None:
            event.Skip()
    
#todo check time against last timestamp
#if (datetime.utcnow() - self.device.lastimestamp) > timedelta (seconds = 100)        
#
    
    def OnEMSConsoleIdle(self, event):
        if self.device.attached:
            self.statusBar1.SetStatusText("Attached:"+str(self.device.device_ssn_string),1)
            self.processSerial(event)
            
            if self.device.device_ssn_string is None:
                self.device.attached=False
                return
            
            if (int(self.device.status['TMR'])):
                #print "DBG:pause"
                self.RUN.SetBitmapLabel(self.pauseb)
            else:
                # print "DBG:play"
                self.RUN.SetBitmapLabel(self.playb)
            self.RUN.Update()
            
            self.TS1.SetValue( self.scaleTempLiteralForGague(self.device.status['TS1']) )
            self.TS2.SetValue( self.scaleTempLiteralForGague(self.device.status['TS2']) )

            if self.device.status['PMS'] is not None:
                self.PMS.SetValue(("ON" in self.device.status['PMS'])
                                  or ("1" in self.device.status['PMS'])
                                  )
            else:
                self.device.sendCMD("PMS?")
            
            if self.device.status['PS1'] is not None:
                self.PS1.SetLabel(self.device.status['PS1'])
            else:
                self.device.sendCMD("PS1?")

            if self.device.status['TRM'] is not None:
                self.TRM.SetLabel(self.device.status['TRM'])
            else:
                self.device.sendCMD("TRM?")

            if self.device.status['SOL'] is not None:
                self.SOL.SetValue(("ON" in self.device.status['SOL'])
                                  or ("1" in self.device.status['SOL'])
                                  )
            else:
                self.device.sendCMD("SOL?")
            
            
            if self.device.status['HSW'] is not None:
                self.HSW.SetValue(("ON" in self.device.status['HSW'])
                                  or ("1" in self.device.status['HSW'])
                                  )
            else:
                self.device.sendCMD("HSW?")

            if (self.device.status['TMP'] is not None
                 and self.tmpIsBeingTracked is False
                ):
                tempAsInt=int(float(str(self.device.status['TMP'])))
                self.TMP.SetValue(int(tempAsInt))
            if (self.device.status['TMS'] is not None
                 and self.tmsIsBeingTracked is False
                ):
                self.TMS.SetValue(int(self.device.status['TMS']))
            self.statusBar1.SetStatusText("TS1:"+str(self.device.status['TS1'])
                                          +"  TS2:"+str(self.device.status['TS2']),3)
        else:
            self.statusBar1.SetStatusText("No device Connected!",1)
            self.OnReconnect()
            
        event.Skip()

    def OnTMPCommandScrollThumbrelease(self, event):
        self.device.sendCMD("TMP!"+str(self.TMP.GetValue()))
        self.tmpIsBeingTracked=False
        event.Skip()

    def OnTMPCommandScrollThumb(self, event):
        self.tmpIsBeingTracked=True
        event.Skip()

    def OnSetTime(self, event):
        self.device.syncTime()
        event.Skip()

    def OnTMSCommandScrollThumbrelease(self, event):
        self.device.sendCMD("TMS!"+str(self.TMS.GetValue()))
        self.tmsIsBeingTracked=False
        event.Skip()
 
    def OnTMSCommandScrollThumb(self, event):
        self.tmsIsBeingTracked=True
        event.Skip()

    def OnHSWCheckbox(self, event):
        if self.HSW.IsChecked():
            self.device.sendCMD("HSW!1")
        else:
            self.device.sendCMD("HSW!0")
        event.Skip()

    def OnPMSCheckbox(self, event):
        if self.PMS.IsChecked():
            self.device.sendCMD("PMS!1")
        else:
            self.device.sendCMD("PMS!0")
        event.Skip()

    def OnSOLCheckbox(self, event):
        if self.SOL.IsChecked():
            self.device.sendCMD("SOL!1")
        else:
            self.device.sendCMD("SOL!0")
        event.Skip()

    def goingUp(self, event):
        self.device.sendCMD("SOL!0")
        self.device.sendCMD("PMS!1")
        event.Skip()
    
    
    def goingDown(self, event):
        self.device.sendCMD("SOL!1")
        self.device.sendCMD("PMS!1")
        event.Skip()

    def pumpOff(self,event):
        self.device.sendCMD("SOL!0")
        self.device.sendCMD("PMS!0")
        event.Skip()


    def OnRUNButton(self, event):
        if (int(self.device.status['TMR'])):
            self.RUN.SetBitmapLabel(self.pauseb)
            self.device.sendCMD("TMR!0")
        else:
            self.RUN.SetBitmapLabel(self.playb)
            self.device.sendCMD("TMR!1")
        self.RUN.Update()
        event.Skip()

    def conlcs(self,key='LCS',verb='?',value=None):
        
        if value is not None:
            self.device.status['LCS']=value
        
        if (("OFF" in self.device.status['LCS'])
            or ("0" in self.device.status['LCS'])
            ): return
        
        waitforit = wx.MessageDialog(None,
                                 'Work Area Not Clear\nCheck area and Press OK to Reset Curtain',
                                 'Light Curtain',
                                 wx.OK | wx.ICON_HAND)
        waitforit.ShowModal()
        self.device.sendCMD("LCR!1")
        waitforit.Destroy()

    def conalt(self,key='ALT',verb='!',value='UNKNOWN'):
        
        waitforit = wx.MessageDialog(None,value,'ALERT!',wx.OK | wx.ICON_EXCLAMATION)
        waitforit.ShowModal()
        waitforit.Destroy()
                                     
    #
    def conwar(self,key='WAR',verb='!',value='?'):
        
        waitforit = wx.MessageDialog(None,value,'WARNING!',wx.OK | wx.ICON_WARNING)
        waitforit.ShowModal()
        waitforit.Destroy()

    def conftl(self,key='FTL',verb='?',value='?'):
        waitforit = wx.MessageDialog(None,
                                 value+'\n OK to Reset Machine',
                                 'FATAL ERROR',
                                 wx.OK | wx.ICON_STOP)
        waitforit.ShowModal()
        self.device.sendCMD("RST!")
        waitforit.Destroy()

    def constp(self,key='STP',verb='?',value='?'):
        waitforit = wx.MessageDialog(None,
                                 
                                     'E-STOP engaged. Check machine and work area'+
                                     'Press OK after resetting switch',
                                     'EMERGENCY STOP',
                                      wx.OK | wx.ICON_STOP)
        waitforit.ShowModal()
        self.device.sendCMD("RST!")
        waitforit.Destroy()
