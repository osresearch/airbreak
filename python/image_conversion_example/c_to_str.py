#!/usr/bin/env python3

def parse_embmp_c(srclines, print_summary=True):
    """Very specifically designed to read the BmpCvtDemo c-file output only"""

    linecnt = 0

    pallete_found = False
    pallete_meta_found = False
    pixels_found = False
    meta_found = False
    done = False

    while done is False:

        #Pallete up first
        if pallete_found is False:
            if srclines[linecnt].startswith("static GUI_CONST_STORAGE GUI_COLOR"):
                linecnt += 2
                
                pallete = ""
                
                #Read up until we hit something
                
                while srclines[linecnt].startswith("#") is False:
                    pallete += srclines[linecnt].rstrip('\n\r')
                    linecnt += 1
        
                pallete_found = True
                pallete = pallete.replace(',', ' ')
                pallete = pallete.split(' ')            
                while '' in pallete: pallete.remove('')
                pallete = [int(p, 0) for p in pallete]

        #pallete size info
        if pallete_meta_found is False:
            if srclines[linecnt].startswith("static GUI_CONST_STORAGE GUI_LOGPALETTE"):            
                pallete_numberentries = int(srclines[linecnt+1].split(",")[0], 0)
                pallete_numbertransp = int(srclines[linecnt+2].split(",")[0], 0)
                pallete_meta_found = True
                
        if pixels_found is False:
            if srclines[linecnt].startswith("static GUI_CONST_STORAGE unsigned char"):
                linecnt += 1
                
                pixels = ""
                while srclines[linecnt].startswith("};") is False:
                    
                    #Remove comment if present at start
                    l = srclines[linecnt].split("*/")[-1]

                    #Remove trailing and starting stuff and spaces
                    l = l.rstrip("\n\r ").strip(" ").replace(" ", "")
                    pixels += l
                    linecnt += 1
                
                pixels = pixels.split(',')
                pixels = [int(p, 0) for p in pixels]
                pixels_found = True
                
        if meta_found is False:
            if srclines[linecnt].startswith("GUI_CONST_STORAGE GUI_BITMAP"):
                #Sanity check user did this thing right
                if "GUI_COMPRESS_RLE" not in srclines[linecnt+4]:
                    raise ValueError("GUI_COMPRESS_RLE not seen in metadata - bailing")
                if "GUI_DRAW_RLE" not in srclines[linecnt+7]:
                    raise ValueError("GUI_DRAW_RLE not seen in metadata - bailing")
         
                meta_xsize = int(srclines[linecnt+1].split(',')[0], 0)
                meta_ysize = int(srclines[linecnt+2].split(',')[0], 0)
                meta_bytesper = int(srclines[linecnt+3].split(',')[0], 0)
         
                meta_found = True
         
        done = pallete_found and pallete_meta_found and pixels_found and meta_found
         
        linecnt += 1
    
    if print_summary:
        print("Parsed image file, results:")
        print("  Size = {x} x {y}".format(x=meta_xsize, y=meta_ysize))
        print("  Pixel size = {} bytes".format(len(pixels)))
        print("  Pallete size = {} bytes".format(len(pallete)))
        
    return (pallete, pallete_numberentries, pallete_numbertransp, pixels, meta_xsize, meta_ysize, meta_bytesper)
    
def c_to_py(filename):
    f = open(filename, "r")
    srclines = f.readlines()
    f.close()
    
    pallete, pallete_numberentries, pallete_numbertransp, pixels, meta_xsize, meta_ysize, meta_bytesper = parse_embmp_c(srclines)
    
    newfile = "# Auto-Generated on some fine day.\n"
    newfile += "# Converted from {}\n".format(filename)
    newfile += "# NOTE: See BmpCvt license from Segger - this output may not be used commercially without a BmpCvt license!!\n\n"
    
    newfile += "pixels = " + str(pixels) + "\n\n"
    
    newfile += "meta_xsize = " + str(meta_xsize) + "\n"
    newfile += "meta_ysize = " + str(meta_ysize) + "\n"
    newfile += "meta_bytesper = " + str(meta_bytesper) + "\n\n"
    
    newfile += "pallete_numberentries = " + str(pallete_numberentries) + "\n"
    newfile += "pallete_numbertransp = " + str(pallete_numbertransp) + "\n"
    newfile += "pallete = " + str(pallete) + "\n\n"
    
    newfile += "#************* Thank you, come again! ****************\n"
    
    outname = filename.replace(".", "_") + ".py"
    f = open(outname, "w")
    f.write(newfile)
    f.close()
    
    print("Wrote output file " + outname)
    
if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description='Convert BmptCvt files to Python')
    
    parser.add_argument('input', metavar='file.c', type=str, help='Input C-file generated')
    args = parser.parse_args()
    
    print("Processing input file " + args.input)
    
    c_to_py(args.input)