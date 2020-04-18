
# About Images

Images are stored in a format from emWin. On boot there are two images:

* 184x84 (LOGO)
* 184x50 (TEXT)

We can easily replace both of them to make it clear this is a modified machine. The following is instructions for doing so.

It's easier to make the images the above sizes, although it should be possible to modify the drawing instructions to use larger images.

# Using BmpCvtDemo

BmpCvtDemo and it's output is freely usable under **non-commercial** purposes. If you will be using this in any commercial manner, you will be required to purchase a license from Segger directly.

Download from [Segger](https://www.segger.com/products/user-interface/emwin/tools/tools-overview/) (registration required) - search on that page for 'Bitmap Converter'.

## Running BmpCvtDemo

1. Open BmpCvtDemo
2. Drag and drop image-file onto BmpCvtDemo window
3. Select `Image -> Convert To -> 6-bit colour` (change depending on image colours, you can use 8-bit options as well).
4.  Select `File -> Save`
5.  Select `Compressed, RLE8` as output format for the main logo, and `Compressed, RLE4` for the 'text area'. This option will only come up if you converted to a supported palette.

If you open the C file, you will see a note at the bottom of it about the file size.

You can see details of the format in the emWin header (at least for structures). Note that versions of emWin are available, for example see [LCD.h which has structs referenced](https://github.com/hrshygoodness/EFM32-Library/blob/master/v2/reptile/emwin/inc/LCD.h).

Often the palette and/or file is larger than the original one in the flash, so we place them elsewhere in flash. But you don't want to waste too much space with these things presumably...

## C to Bytes

The script `c_to_py.py` will convert this to a Python file that the  patcher expects. Run the script as follows:

	python c_to_py.py example1_umbrella_text.c

This outputs a file called `example1_umbrella_text_c.py`. Leave the file where it is.

Back in `airsense-patcher.py`, find the function `patch_logos()`. At the start of this function there is the import of the two logo & text python files. Adjust these files as needed.

# Example

Example files used to generate the fictional Umbrella Corporation logo are shown in the 3 stages (original file, C-file, and python-file) in the repository.