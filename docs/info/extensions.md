![`sprintf` on the screen](/images/sprintf.jpg)


# Writing extensions

There are currently two example extensions: [`breath.c`](https://github.com/osresearch/airbreak/blob/master/breath.c) and [`graph.c`](https://github.com/osresearch/airbreak/blob/master/graph.c).
The breathing mod implements a Pressure Control Ventilator with programmable
pressure and respiration rate.  The graphing mod adds a strip chart for the
current commanded pressure and the actual output pressure.

This is similar to [Magic Lantern](https://magiclantern.fm), since we use the
existing vendor firmware as a library with functions at fixed addresses and fit into
the empty space around the flash image.  The Ghidra XML file can be converted into
the `stubs.S` that provides labels at the fixed locations.

More documentation needs to be written.


Ghidra files.

Data structures.

Configuration and variables.

GUI library is based on the closed
source emwin, although [the reference manual is
available](https://www.nxp.com/docs/en/user-guide/UM03001_emWin5_5.pdf)
and can be used to derive the headers and structures required to interface
with the library.

GPIO library is the STM32 Standard Peripheral Library.

There is a hard-float unit in the CPU, but some float/double ops seem
to cause a fault.  Be sure that all of your constants have an explicit
float type like `0.1f`, otherwise gcc will use doubles.
