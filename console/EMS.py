#!/usr/bin/python
"""
*-----------------------------------------------------------------------EMS.py
*
* Python Class to simplify interaction between the embedded monitor sytem and 
* programs to monitor and control it, the class uses a dispatch table to 
* define the actions. .
*
* Author: Donald Delmar Davis
*
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
*-----------------------------------------------------------------------------
*
*
"""

import re
import string
import datetime
import serial
import os
import os.path
import shutil
import platform
import sys
import time
import re

def list_serial_ports():
  def full_port_name(portname):
    """ Given a port-name (of the form COM7, COM12, CNCA0, etc.) returns a full
        name suitable for opening with the Serial class.
        http://eli.thegreenplace.net/2009/07/31/listing-all-serial-ports-on-windows-with-python/
    """
    m = re.match('^COM(\d+)$', portname)
    if m and int(m.group(1)) < 10:
        return portname
    return '\\\\.\\' + portname

  plat_sys = platform.system()
  plat_bits = platform.architecture()[0]

  if plat_sys == 'Windows':
    import _winreg as reg
    p = 'HARDWARE\\DEVICEMAP\\SERIALCOMM'
    k = reg.OpenKey(reg.HKEY_LOCAL_MACHINE, p)
    possible_paths = []
    i = 0
    while True:
        try:
            possible_paths.append(full_port_name(reg.EnumValue(k, i)[1]))
            i += 1
        except WindowsError:
            break
  else:
    best_candidate=None
    if plat_sys == 'Linux':
        file_prefix='ttyACM'
        if os.path.exists('/dev/arduinoZero'):
            best_candidate='/dev/arduinoZero'
      
    elif plat_sys == 'Darwin':
       file_prefix='cu.usbmodem'
    
    possible_paths = [os.path.join('/dev', x) for x in os.listdir('/dev') \
                          if x.startswith(file_prefix)]
    if best_candidate is not None:
        possible_paths.insert(0, best_candidate)

  if len(possible_paths) == 0:
    return None
  return possible_paths
'''
    "SYN"  "ACK"  "NAK"  "SWV"  "HWV"  "GIT"  "MEM"  "SSN"  "HLP"  "FTL"  "ALT"  "WAR"  "INF" \
    "DBG"  "LOG"  "STC"  "DVL"  "LVL"  "RST"  "BLD"  "STP"  "LCS"  "LCR"  "NOW"  "TIM"  "TS1" \
    "TS2"  "TS3"  "AMT"  "AMH"  "RED"  "H1S"  "H1V"  "H1P"  "H1I"  "H1D"  "H2S"  "H2V"  "H2P" \
    "H2I"  "H2D"  "PMP"  "PMS"  "PSP"  "PSI"  "PSD"  "PS1"  "TMS"  "TMR"  "TRM"  "SOL"  "HOM" \
    "CMD"  "MLD"  "SOD"  "EOD"  "NOP"
'''

class EMS:

    def __init__(self, portname=None, baudrate=9600):
        self.ser=None
        self.attached=False
        self.lastcontact=None
        self.lastTimestamp=None
        self.command_state=-1
        self.acknowleged=False
        self.dispatch_table={
            'SSN':self.ssn,'HWV':self.hwv,'SWV':self.swv,'LOG':self.log,
            'NAK':self.nak,'ACK':self.ack,'CMD':self.cmd,'DBG':self.dbg,
            'SOL':self.sol,'TIM':self.tim
        }
        self.status={
            'TS1':None,'TS2':None,'AMT':None,'PS1':None,'AMH':None,
            'HOM':None,'PMS':'0' ,'SOL':'0' ,'TMP':None,'TMS':None,
            'TMR':'0' ,'TRM':'0' ,'LCR':'0' ,'LCS':'0' ,'HSW':'0'
        }
        
        for var in self.status.keys():
           self.dispatch_table[var]=self.update_state_variable

        self.open(portname,baudrate)
        

    def yank_timestamp(self,value):
        if value is None:
            return None
        try:
            
            ll=value.split(',',2)
            #print value,"->",ll,len(ll)
            if len(ll) == 1:
                self.lastcontact=datetime.datetime.utcnow()
                return ll[0]
            else:
                self.lastcontact=datetime.datetime.utcnow()
                self.lastTimestamp=ll[0]
                return ll[1]
        except Exception as e:
            print "error in yank timestamp:", str(e)
            return None
            
    def update_state_variable(self,key,verb,value):
        self.status[key]=self.yank_timestamp(value)
        #should maybe flag the update somewhere
        #may want ot return something.
        
    def set_dispatch_function(self,key,d_fun):
        #should check key value and callable here....
        self_dispatch_table[key]=d_fun
        
    def log(self,key='LOG',verb=':',value=None):
        if value is not None:
             print "LOG:"+value.rstrip('\r\n') 
        return value

    def sol(self,key='SOL',verb=':',value=None):
        if value is not None:
           print "DBG:"+key+' '+value.rstrip('\r\n')
        return value
    
    def alt(self,key='ALT',verb='?',value=None):
        if value is not None:
            print key+":"+value.rstrip('\r\n')
        return value

    def ssn(self,key='SSN',verb='?',value=None):
        if (value is not None):
            ssn=self.yank_timestamp(value).rstrip('\r\n')
            if ( len(ssn)>9  and all(char in string.hexdigits for char in ssn) ):
                self.device_ssn_string=ssn
            else:
                self.sendCMD("SSN?") #we got garbage ask again
        return self.device_ssn_string

    def hwv(self,key='HWV',verb='?',value=None):
        if value is not None:
            self.device_hwv_string=self.yank_timestamp(value)
        return self.device_hwv_string

    def swv(self,key='SWV',verb='?',value=None):
        if value is not None:
            self.device_swv_string=self.yank_timestamp(value)
        return self.device_swv_string

    def nak(self,key='NAK',verb=':',value=None):
        if value is not None:
            self.yank_timestamp(value)
            self.acknowleged=True
        return self.acknowleged
    
    def ack(self,key='ACK',verb='?',value=None):
        if value is not None:
            self.yank_timestamp(value)
            self.acknowleged=True
        return self.acknowleged

    def cmd(self,key='CMD',verb='?',value=None):
        if value is not None:
            self.command_state= "ON" in self.yank_timestamp(value)
        return self.command_state

    def stp(self,key='STP',verb='?',value=None):
        print key,"DBG:("+key+")not implimented FIX ME!!!"
    
    def dbg(self,key='DBG',verb=':',value=None):
        if value is not None:
            print key+":"+value
        #return something probably
        
    def sendCMD(self,cmd=None):
        if cmd is not None and self.ser is not None:
            self.ser.write(cmd+'\r\n')
        #return something probably
        
    def nop(self,key='NOP',verb=':',value=None):
        print key,"DBG:("+key+")not implimented!!!"
    
    def tim(self,key='TIM',verb='?',value=None):
        print key,"DBG:("+key+")stub!!! should look for clock skew"
    
    def syncTime(self,key='NOP',verb=':',value=None):
        self.sendCMD("TIM!"+datetime.datetime.now().strftime("%m/%d/%Y %H:%M:%S"))
    
    
    def open(self,portname,baudrate,printDebug=True):
        attempts=2
        line=''
        self.version_string=None
        self.device_ssn_string=None
        self.device_swv_string=None
        self.device_dbg_level=None
        self.portname=None
        self.lastconact=None
        self.initialContact=None
        self.ser=None
        while attempts and self.portname is None:
          ports=list_serial_ports()
          if ports is None:
            break
          else:
            for port in ports:
                try:
                    self.initialContact=[]
                    ser=serial.Serial(port,9600,timeout=1)#,dsrdtr=True,rtscts=True)
                    if not ser.isOpen:
                        continue
                    ser.flushInput()
                    time.sleep(.1)
                    try_no=1
                    while not ser.inWaiting():
                        time.sleep(.2)
                        try_no=try_no+1
                        if try_no > 10:
                            break
                            
                    try_no=1
                    ser.write('SYN!\r\nSSN?\r\nSWV?\r\nHWV?\r\n')
                    while not ser.inWaiting():
                        time.sleep(.25)
                        try_no=try_no+1
                        if try_no > 10:
                            break
                    
                    while ser.inWaiting():                            
                        line=unicode(ser.readline(100).rstrip('\r\n'), errors='ignore')
                        line=line.encode('ascii','ignore')
                        if len(line) > 3 and line[3] == ':' :
                            self.dispatch(line)
                        else:
                           print "DBG:GARBAGE >>"+line+"<<"
                           #check stream for rev 1 hardware
                        self.initialContact.append(line)
                        time.sleep(.35)
                    
                except Exception as e:
                    print line
                    print "Error in open :", str(e)
                    
                if self.device_swv_string is not None:
                    print "DBG:Found an EMS : version =", self.device_swv_string
                    print "DBG:SSN =", self.device_ssn_string
                    self.portname=port
                    self.ser=ser
                    break
                if self.portname is None:
                    try:
                        ser.close()
                    except Exception as e:
                        print "error trying to close", str(e)
                                
                attempts=attempts-1
        self.attached = (self.ser is not None)
    
    def dispatch(self,buffer):
        if buffer is None:
            print "DBG:short read"
            return
        if (len(buffer) < 4):
            if (len(buffer.rstrip('\r\n'))):
                print "DBG:GARBAGE>>>"+buffer.rstrip('\r\n')+"<<<"
            return
        key=buffer[0:3]
        verb=buffer[3]
        if len(buffer) > 4:
            value=buffer[4:].rstrip('\r\n')
        else:
            value=''
        self.dispatch_table.get(key,self.nop)(key,verb,value)
"""
*----------------------------------------------------------------------test code
* 
*
""" 
 
    
if __name__ == "__main__":
    
    
    import httplib, urllib
    
    def log_to_app(key,verb=':',value=None):
        if value is not None and device.device_ssn_string is not None : 
            try:
                print "LOG:(" + device.device_ssn_string + ")" + value
                ssn=device.device_ssn_string
                (timestamp,ambient,chiller1,chiller2,air1,air2,chiller,
                    fan,soh,state) = value.split(',')
                #print "SANITY CHECK", ssn, timestamp, ambient
                params = urllib.urlencode({#'ssn': ssn,
                                            'timestamp':timestamp,
                                            'ts1': ambient,
                                            'ts2': chiller1,'ts3': chiller2,
                                            'ts5': air2,
                                            'chiller': chiller, 
                                            'fan': fan,
                                            'soh': soh,
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
    
    def test_stc (key,verb,value):
        print key,verb,value,device.device_ssn_string

       
    device=EMS()
    if device.ser is None:
        print "FOUND NO EMS DEVICE"
        exit()
    device.dispatch_table['LOG']=log_to_app
    while True:
        if device.ser.inWaiting():
            line=device.ser.readline(100).rstrip('\r\n')
            line=line.encode('ascii', 'ignore')
                        
            device.dispatch(line)
    
