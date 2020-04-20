#!/usr/bin/env python3

# This work was not produced in affiliation with any of the device manufactures and is,
# and is intended to be, an independent, third-party research project.
#
# This work is presented for research and educational purposes only. Any use or reproduction
# of this work is at your sole risk. The work is provided “as is” and “as available”, and without
# warranties of any kind, whether express or implied, including, but not limited to, implied
# warranties of merchantability, non-infringement of third party rights, or fitness for a
# particular purpose.
#

try:
    import pylink
except ImportError:
    pylink = None

from asmonitor_serial import ASSerialMonitor

class ASMonitor_JLink(object):
    def connect():
        if pylink is None:
            raise NotImplementedError("pylink module did not import - try installing it")
        jlink = pylink.JLink()
        jlink.open()
        jlink.set_tif(pylink.enums.JLinkInterfaces.SWD)
        jlink.connect('STM32F405VG', speed=4000)
        
    def return_mem32_unpacked(addrstart, num, unpackstr):
        mem = jlink.memory_read(addrstart, num*4)
    
        lst = []
    
        for i in range(0, num):
            v = struct.unpack(unpackstr, bytes(mem[(i*4):(i*4 + 4)]))
            lst.append(v[0])
        
        return lst

class ASMonitor_Serial(object):
    def connect():
        pass

    def return_mem32_unpacked(addrstart, num, unpackstr):
        mem = jlink.memory_read(addrstart, num*4)
    
        lst = []
    
        for i in range(0, num):
            v = struct.unpack(unpackstr, bytes(mem[(i*4):(i*4 + 4)]))
            lst.append(v[0])
        
        return lst

class ASMonitor(object):
    
    #TODO - this seems like wrong way, but anyway works for demos
    tm_variables = {
        0x2D : "Target Pressure",
        0x8F : "Leak(GUI)", #From GUI - GUI stops at 120 however
        0xB9 : "VT(GUI)",   #From GUI
        0xBA : "RR(GUI)"    #From GUI
    }
    
    def __init__(self, lowlevel):
        self.ll = lowlevel


    def return_floats(addrstart, num_fps=1):
        """Return FP variables from array"""
        
        return self.ll.return_mem32_unpacked(addrstart, num_fps, "<f")
        
    def return_ints(addrstart, num_ints=1):
        """Return int variables from array"""
        
        return self.ll.return_mem32_unpacked(addrstart, num_ints, "<i")
        
        
    def return_therapy_floats(start=0, num_fps=255):
        """Return FP variables from therapy manager"""
        
        return self.ll.return_floats(0x2000e948 + (start*4), num_fps)

    def return_therapy_ints(start=0, num_ints=255):
        """Return int variables from therapy manager"""
        
        return self.ll.return_mem32_unpacked(0x2000e750 + (start*4), num_ints, "<i")