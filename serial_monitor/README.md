# Replacement Serial Monitor

The AirXXXXX units have a serial protocol on the USART3 pins, which are available via the accessory port. See the example adapter PCB in `docs/adapter_pcbs` to get easy access to this.

This code *disables the built-in serial protocol* and replaces it with a full binary protocol for read/write to memory. This allows usage of the device without JTAG connection.

Future extensions include register read/write & address jumps.

Eventually it could be worth implementing a proper GDB debugger, which could be done by port [MRI](https://github.com/adamgreen/mri) to this platform.

## Notes on Implementation

This is implemented with interrupts. It turns out the units have DMA by default, which should be used in the future as reduces the CPU overhead. However, the IRQ version is tested and works.

Note that the setup is done by replacing one of the functions that processes serial commands. Thus you need to send an "original" command to the device. This is most easily done with `Uv` for example.

## Protocol

The protocol is something like this:

	[SOF] [CMD] [ DATA (variable) ] [ESC] [EOT] [CRC]

* SOF (Start of Frame) is fixed `0xFE`.
* CMD is a memory read or write
* Data is address & data information (address only for read).
* ESC is escape character `0x1B`.
* EOT (End of Transmission) is `0x02`.
* CRC is a CRC8 of original values of `CMD` + `DATA`.

Note that byte stuffing is used to ensure that `0xFE` never exists in the frame itself. `0xFE` will be replaced with `1B 01`. See Python implementation for more details.

## Python Implementation

See the 'python' directory for the computer-side implementation. This includes reading from various interesting variables.