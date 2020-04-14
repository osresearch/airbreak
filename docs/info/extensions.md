![`sprintf` on the screen](/images/sprintf.jpg)

# Writing extensions

To be written.  Similar to [Magic Lantern](https://magiclantern.fm), since we use the
existing vendor firmware as a library with functions at fixed addresses and fit into
the empty space around the flash image.  The ghidra XML file can be converted into
the `stubs.S` that provides labels at the fixed locations.

Ghidra files.

Data structures.

Configuration and variables.

GUI library is based on the closed
source emwin, although [the reference manual is
available](https://www.nxp.com/docs/en/user-guide/UM03001_emWin5_5.pdf)
and can be used to derive the headers and structures required to interface
with the library.

GPIO library is the STM32 Standard Peripheral Library.

