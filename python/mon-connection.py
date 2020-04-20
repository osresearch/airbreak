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

# Very small/simple monitor to match embedded firmware. Useful for monitoring/dumping variables

import serial
import struct

class ASMonitor(object):
    """ Very simple monitor to allow read/write to memory space """

    ESC = 0x1B
    SOF = 0xFE
    ESC_SOF = 0x01
    ESC_EOT = 0x02
    ESC_ESC = 0x03
    CMD_MEMWR = 0x10
    CMD_MEMRD1 = 0x11
    RSP_MEMRD1 = 0x12
    
    CRC_8_TABLE = [
    0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
    35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
    190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
    70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
    219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
    17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
    50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
    87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53
    ]

    def write_mem32(self, address, value):
        """Write a 32-bit value to address"""
        cmd = [self.CMD_MEMWR]
        cmd += struct.pack("<I", address)
        #print(cmd)
        cmd += struct.pack("<I", value)
        
        cmd = self.pack_and_crc(cmd)
        #print(cmd)
        self._write(cmd)

    def read_mem32(self, address):
        """Read a 32-bit value from address"""
        cmd = [self.CMD_MEMRD1]
        cmd += struct.pack("<I", address)
        
        cmd = self.pack_and_crc(cmd)
        self._write(cmd)
        
        #Read frame
        cmd = self.read_until_eot()
        cmd = self.unpack_and_crc(cmd)
        
        if cmd[0] != self.RSP_MEMRD1:
            raise IOError("Out of sync")
        
        val = struct.unpack("<I", bytes(cmd[1:5]))
        return val[0]

    def read_float(self, address):
        """Read a Floating Point value from address"""
        cmd = [self.CMD_MEMRD1]
        cmd += struct.pack("<I", address)
        
        cmd = self.pack_and_crc(cmd)
        self._write(cmd)
        
        #Read frame
        cmd = self.read_until_eot()
        cmd = self.unpack_and_crc(cmd)
        
        if cmd[0] != self.RSP_MEMRD1:
            raise IOError("Out of sync")
        
        val = struct.unpack("<f", bytes(cmd[1:5]))
        return val[0]
        

    def open(self, portname):
        """Open a connection to the target device"""
    
        self.ser = serial.Serial(portname, 57600, timeout=0.25)
        
        #Need to call existing function once to switch over to our monitor
        self.ser.write(b"Uu")
        self.ser.flush()
        self.ser.read(100)
        
        #Update baud - 2D9 is baud rate of 57600
        # self.write_mem32(0x40004808, 0x000002D9)
        
    def crc8(self, data):
        """CRC-8 implementation that matches embedded one"""
        crc = 0
        for d in data:
            crc = self.CRC_8_TABLE[crc ^ d]
        return crc
        
    def _write(self, data):
        #print(data)
        self.ser.write(data)
        
    def read_until_eot(self):
        """Read until the EOT sequence is seen"""
        data = self.ser.read_until(b"\x1B\x02")
        if data.endswith(b"\x1B\x02") is False:
            #print(" ".join(["%02x"%d for d in data]))
            raise IOError("Timeout - never saw 1B 02 sequence")
        return data
        
    def unpack_and_crc(self, input):
        """Take byte sequence, perform unstuffing as needed"""
        
        data = []
        
        idx = 0
        while input[idx] != self.SOF:
            idx += 1
        
        idx += 1
        
        while idx < len(input):
            if input[idx] == self.ESC:
                if input[idx+1] == self.ESC_EOT:
                    #Break out
                    break
                elif input[idx+1] == self.ESC_ESC:
                    data.append(self.ESC)
                elif input[idx+1] == self.ESC_SOF:
                    data.append(self.SOF)
                else:
                    raise IOError("Invalid escape: %x"%(input[idx+1]))                
                idx += 2
            else:
                data.append(input[idx])
                idx += 1

        if self.crc8(data) != 0:
            raise IOError("CRC8 Error")

        return data[:-1]

    def pack_and_crc(self, input):
        """Take byte sequence, perform stuffing as needed"""
        output = [self.SOF]
        
        crc = self.crc8(input)
        
        input = input[:]
        input += [crc]
        
        for b in input:
            if b == self.SOF:
                output += [self.ESC, self.ESC_SOF]
            
            elif b == self.ESC:
                output += [self.ESC, self.ESC_ESC]
           
            else:
                output += [b]        
        
        output += [self.ESC, self.ESC_EOT]        
        #print(output)
        return output

    def close(self):
        self.ser.close()

if __name__ == "__main__":
    AS = ASMonitor()
    AS.open("COM57")

    #AS.write_mem32(0x20001000, 0xEFEF1B1B)
    AS.write_mem32(0x20001F00, 0x11223344)

    base = 0x20000000
    for i in range(0, 1000, 4):
        print("%08X: %08X"%(0x20000000+i, (AS.read_mem32(0x20000000+i))))

    #Try some floats
    for i in range(0, 1000):
        print(AS.read_float(0x200157cc))

    #import time

    #start = time.time()
    #for i in range(0, 1000):
        #AS.write_mem32(0x20001000, i)
    #    AS.read_mem32(0x20001000)
    #end = time.time()

    #print(end-start)

    AS.close()