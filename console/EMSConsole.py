#!/usr/bin/env python
#Boa:App:BoaApp
"""
*--------------------------------------------------------------EMSConsole.py
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
*
*
"""
import wx
import sys
from EMS import EMS
import consoleFrame

modules ={u'consoleFrame': [1, 'Main frame of Application', u'consoleFrame.py']}

import httplib, urllib
"""---------------------------------------------------------------------------
* sample aditional handler for EMS
---------------------------------------------------------------------------"""
def log_to_google_app(key,verb=':',value=None):
    device=application.main.device
    if value is not None and device.device_ssn_string is not None : 
        try:
            #print "LOG:(" + device.device_ssn_string + ")" + value
            ssn=device.device_ssn_string
            (timestamp,ambient,chiller1,chiller2,air1,air2,chiller,
                fan,soh,state) = value.split(',')
            #print "SANITY CHECK", ssn, timestamp, ambient
            params = urllib.urlencode({ 'timestamp':timestamp,
                                        'ts1': ts1,
                                        'ts2': ts2,
                                        'ps1': ps1,
                                        'state': state
                                        })
            headers = {"Content-type": "application/x-www-form-urlencoded",
                       "Accept": "text/plain"}
            conn = httplib.HTTPConnection("xms-python.appspot.com")
        #    need to deal with bogus ssn value
            conn.request("POST", "/log?ssn="+ssn, params, headers)
            response = conn.getresponse()
            print "DBG:"+str(response.status),response.reason,
            data = response.read()
            print data.rstrip('\r\n')
            conn.close()
        except Exception as e:
            print "DBG:NETWORK?"+str(e)
#
# ****** stubb ******
#
def check_for_clock_skew(key,verb=':',value=None):
    if value is not None:
            self.main.emsClockSkew=0 #calculate skew.


class EMSApp(wx.App):
    def OnInit(self):
        self.main = consoleFrame.create(None)
        self.main.SetBackgroundColour("White")
        self.main.Maximize()
        self.main.device=EMS()
        #self.main.device.dispatch_table['LOG']=log_to_google_app
        # override built ins with gui specific handler
        # would preffer these go into frame but am having chicken egg issues...
        self.main.device.dispatch_table['LCS']=self.main.conlcs
        self.main.device.dispatch_table['STP']=self.main.constp
        self.main.device.dispatch_table['FTL']=self.main.conftl
        self.main.device.dispatch_table['ALT']=self.main.conalt
        self.main.device.dispatch_table['WAR']=self.main.conwar
        self.main.device.sendCMD("PS1?\r\nLCS?\r\nSOL?\r\nPMS?\r\nTS1?\r\nTS2?")
        self.main.Show()
        self.SetTopWindow(self.main)
        return True

def main():
    global application
    application = EMSApp(0)
    application.MainLoop()

if __name__ == '__main__':
    main()
