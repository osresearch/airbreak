#!/usr/bin/env python3

import crcmod
import crcmod.predefined
import os.path
import struct

class ASFirmware(object):
    """Patch firmware from device with various changes"""

    def __init__(self, file):
        self.fw = file.read()
        self.fw = list(self.fw)
        self.crcfunc = crc_func = crcmod.predefined.mkCrcFun('crc-ccitt-false')
        
        self.validate()
        
    def validate(self):
        """Validate the input file looks OK"""
            
        #Step 1: Check CRCs seem OK
        crc1 = self.crcfunc(bytes(self.fw[0x0:0x4000]))
        crc2 = self.crcfunc(bytes(self.fw[0x4000:0x40000]))
        crc3 = self.crcfunc(bytes(self.fw[0x40000:0xFFFFFF]))
        
        if crc1 != 0 or crc2 != 0 or crc3 != 0:
            print("CRC 0-4000: %x"%(crc1))
            print("CRC 4000-40000: %x"%(crc2))
            print("CRC 40000-FFFFFF: %x"%(crc3))
            raise IOError("CRC in firmware not as expected")
            
        #Step 2: Find version numbers
        self.str_loader_ver = bytes(self.fw[0x3F80:0x3F90]).decode()
        self.str_model_number = bytes(self.fw[0x4020:0x4027]).decode()
        self.str_model_name = bytes(self.fw[0x4030:0x404F]).decode()
        self.str_fw_ver = bytes(self.fw[0x40000:0x4000B]).decode()
        
        print("Firmware Info: ")
        print("  Loader Version   " + self.str_loader_ver)
        print("  Catalog No.      " + self.str_model_number)
        print("  Model Name       " + self.str_model_name)
        print("  Main SW Version  " + self.str_fw_ver)
        
    def fix_crcs(self):
        """Update CRCs in the file"""
        new_crc = self.crcfunc(bytes(self.fw[0x0:0x3FFE]))
        self.fw[0x3FFE] = new_crc >> 8
        self.fw[0x3FFF] = new_crc & 0xff
        
        new_crc = self.crcfunc(bytes(self.fw[0x4000:0x3FFFE]))
        self.fw[0x3FFFE] = new_crc >> 8
        self.fw[0x3FFFF] = new_crc & 0xff
        
        new_crc = self.crcfunc(bytes(self.fw[0x40000:0xFFFFE]))
        self.fw[0xFFFFE] = new_crc >> 8
        self.fw[0xFFFFF] = new_crc & 0xff
        
    def find_bytes(self, dataseq):
        """Find location of byte sequence in FW"""
        
        i1 = bytes(self.fw).find(bytes(dataseq))
        i2 = bytes(self.fw).rfind(bytes(dataseq))
        
        if i1 != i2:
            raise ValueError("Passed sequence is not unique! Found at 0x%x and 0x%"%(i1, i2))

        if i1 == -1:
            raise ValueError("Passed sequence not found")

        return i1

    def patch(self, patchdata, addr=None, dataseq=None, hash=None, verbose=True):
        """Updates firmware data with patchdata, based on address, sequence, or hash of sequence"""
        
        #I love Python3(TM)
        patchdata = list(bytes(patchdata))
        
        patchlen = len(patchdata)
        
        #Use simple method - fixed address patch
        if addr:
            pass
        
        elif dataseq:
            addr = self.find_bytes(dataseq)
            
        elif hash:
            raise NotImplementedError("Not yet done")
            
        else:
            raise ValueError("Need to specify one of the patch methods")

        if verbose:
            print("Patching %d bytes at 0x%x"%(patchlen, addr))
        self.fw[addr:(addr+patchlen)] = patchdata

    def find_flash_room(self, length_needed, start=0x4000, start_mod=0x100, reserve=True):
        """Find at least length_needed bytes of 0xFF in flash we can hopefully re-use."""
        
        address = -1
        
        start_padding = 32
        end_padding = 256
        
        trying = True
        
        while trying:
            candidate = bytes(self.fw[start:]).find(bytes([0xff] * (length_needed + start_padding + end_padding)))
            if candidate < 0:
                raise ValueError("No more room :(")
            candidate += start
            candidate += start_padding
            
            #Round up to requested start position, check it will still work
            while candidate % start_mod != 0:
                candidate += 1
            
            if self.fw[candidate:(candidate+length_needed)] != [0xFF]*length_needed:
                print("Oops... try again")
                start = candidate
            else:
                address = candidate
                trying = False
        
        if address < 0:
           raise ValueError("Failed to find space?")
        
        print("Found space at " + str(hex(address)))
        
        if reserve:
            print("Reserving %d bytes"%length_needed)
            self.fw[candidate:(candidate+length_needed)] = [0xBA] * length_needed
        
        return address
        
    def patch_image(self, structaddr, palletaddr, pixeladdr, image):
        #X size
        self.fw[(structaddr + 0):(structaddr + 2)] = list(struct.pack('H', image.meta_xsize))

        #Y size
        self.fw[(structaddr + 2):(structaddr + 4)] = list(struct.pack('H', image.meta_ysize))

        #'BytesPerLine' size
        self.fw[(structaddr + 4):(structaddr + 6)] = list(struct.pack('H', image.meta_bytesper))

        
        # We leave bitsperpixel alone - should be '0'
        #self.fw[structaddr + 6]
        #self.fw[structaddr + 7]
        
        #Pointer to pixels
        self.fw[(structaddr + 8):(structaddr + 12)] = list(struct.pack('I', pixeladdr + 0x08000000))
        
        #Pointer to pallete
        self.fw[(structaddr + 12):(structaddr + 16)] = list(struct.pack('I', palletaddr + 0x08000000))
        
        #Pointer to function for drawing/decoding (not changed)
        #self.fw[(structaddr + 16):(structaddr + 24)]
        
        #Copy pixel data over as well
        self.patch(image.pixels, pixeladdr)
        
        #Pallete needs a little support struct to feel better
        self.fw[(palletaddr + 0):(palletaddr + 4)] = list(struct.pack('I', image.pallete_numberentries))
        self.fw[(palletaddr + 4):(palletaddr + 8)] = list(struct.pack('I', image.pallete_numbertransp))
        self.fw[(palletaddr + 8):(palletaddr + 12)] = list(struct.pack('I', palletaddr + 16 + 0x08000000))

        #Copy pallete over where we expect it
        for i in range(0, len(image.pallete)):
            self.fw[(palletaddr + 16 + (i*4)):(palletaddr + 16 + (i*4 + 4))] = list(struct.pack('I', image.pallete[i]))

    def write_output(self, filename, overwrite=False):
        if os.path.exists(filename) and (overwrite == False):
            raise IOError("File " + filename + "exists already.")
    
        f = open(filename, "wb")
        f.write(bytes(self.fw))
        f.close()

def unlock_ui_limits():
    asf.patch(b'\xdc\x05\x00\x00\x32\x00', 0x4fa8)
    asf.patch(b'\xdc\x05\x00\x00\x32\x00', 0x4fc4)
    asf.patch(b'\xdc\x05\x00\x00\x32\x00', 0x7eb0)
    asf.patch(b'\xdc\x05\x00\x00\x32\x00', 0x7ee8)
    asf.patch(b'\xdc\x05\x00\x00\x32\x00', 0x7ecc)


def extra_debug():
    # set config variable 0xc value to 4 == enable more debugging data on display
    # if you set it to \x0f it will enable four separate display pages of info in sleep report mode
    asf.patch(b'\x04', 0x84a8)

def extra_modes():
    # add more mode entries, set config 0x0 mask to all bits high
    # default is 0x3, which only enables mode 1 (CPAP) and 2 (AutoSet)
    # ---> This is the real magic <---
    asf.patch(b'\xff\xff', 0x8590)

def extra_menu():
    #try enabling extra menu items
    asf.patch(b'\x01\x20', 0x66470)

# If you want all menu items to always be visible, let this section run
def all_menu():
    # force status bit 5 always on -- always editable
    asf.patch(b'\x01\x20', 0x6e502)
    # force status bit 4 always on -- this makes all the inputs show up, regardless of mode
    asf.patch(b'\x01\x20', 0x6e4c4)


def gui_config ():
    # enable all of the editable options in the settings menu
    # by turning on bit 1 of the config entries.  All of these variables
    # are listed in the gui_create_menus function
    GUI_CONFIG=0x4ef4
    GUI_CONFIG_SIZE=0x1c
    GUI_CONFIG_OFFSET=30
    
    for var in [0x2f, 0x1ec, 0x1ed, 0x24, 0x25, 0x1d3,
                0x1d6, 0x1d5, 0x1d7, 0x26, 0x1d9, 0x1e0,
                0x1e1, 0x1e2, 0x1e5, 0x1e4, 0x1e6, 0x1e7,
                0x1e9, 0x1ea, 0x1eb]:
        addr = GUI_CONFIG + (var - GUI_CONFIG_OFFSET) * GUI_CONFIG_SIZE
        asf.patch(b'\x07\x00', addr)

def patch_logos():

    #Change these to adjust logos, rest should work automatically.
    #NB - be sure of settings when saving file:
    #     'text' was exported with `Compressed, RLE4`
    #     'logo' was exported with `Compressed, RLE8`
    import image_conversion_example.example1_umbrella_logo_c as logo
    import image_conversion_example.example1_umbrella_text_c as text
    
    # Find somewhere to stash our stuff in the flash memory
    # NOTE: Pallet is in 32-bit, and need room for struct stuff around pallete
    pallete_addr = asf.find_flash_room(len(logo.pallete*4)+32, reserve=True) 
    pixels_addr = asf.find_flash_room(len(logo.pixels), reserve=True)
    
    # Find the location of the original wave
    setting_loc = asf.find_bytes([0xb8, 0x00, 0x54, 0x00, 0xb8, 0x00, 0x00, 0x00])
    
    asf.patch_image(setting_loc, pallete_addr, pixels_addr, logo)    
    
    # Find somewhere to stash our stuff in the flash memory
    # NOTE: Pallet is in 32-bit, and need room for struct stuff around pallete
    pallete_addr = asf.find_flash_room(len(text.pallete*4)+32, reserve=True) 
    pixels_addr = asf.find_flash_room(len(text.pixels), reserve=True)
    
    # Find the location of the original text
    setting_loc = asf.find_bytes([0xB8, 0x00, 0x32, 0x00, 0x5c, 0x00, 0x00])
    
    asf.patch_image(setting_loc, pallete_addr, pixels_addr, text)

if __name__ == "__main__":
    b = open("../stm32.bin", "rb")
    asf = ASFirmware(b)
    b.close()

    #TODO - need to do SHA256 test, or modify addresses to be more generic

    asf.patch(b'\xc0\x46', 0xF0)
    asf.patch(b'HACKED!', 0x17500)
    asf.patch(b'NOT FOR USE\x00', 0x1A540)
    asf.patch(b'WARNING! WARNING! Ventilator test firmware: Not for humans!\x00', 0x1B860)

    #patch_code()
    unlock_ui_limits()
    extra_debug()
    extra_modes()
    extra_menu()
    #all_menu()
    gui_config()

    patch_logos()

    asf.fix_crcs()
    asf.write_output("../stm32-unlocked.bin", overwrite=True)
