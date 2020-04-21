# Serial Protocol

The unit has a serial port at 57600 baud available on accessory-board (see [pinouts](pinouts.md) for pinouts, and [adapter pcbs](../adapter_pcbs) for physical information).

This collects some in-progress notes. If you just need to talk a unit for experimentation, you might consider instead the working 'serial monitor' replacement code until this is figured out more.

This is assumed to be used with the [TX Link](https://www.resmed.com/epn/dam/documents/products/titration/tx-link/quick-setup-guide/tx-link_quick-setup-guide_glo_eng.pdf) device. Information is only available on the S9 version, but the interface connector looks to be the same (not confirmed).

## Protocol Notes

The device will respond to some commands starting with a `U`. The code can be seen checking for a 0x55 in parts, so this makes sense as some sort of Sync character. Sending for example:

	UV

Will have the unit respond with:

	UE010 = 6011807B

The `807B` is a CRC-16 of the preceding ASCII data:
	
	import crcmod
	import crcmod.predefined
	crc_func = crcmod.predefined.mkCrcFun('crc-ccitt-false')
	crc = crc_func(b"UE010 = 6011")
	print(str(hex(crc)))

Sending `U` + most characters seems to trip the above response. Some do not respond however suggesting there is "subcommands" that are waiting for more data:

	UQA seems to give no response.
	UP{A-T,V-Z,0-9} seems to give no response.
	ULA seems to give no response.

The various "response codes" can be seen in the firmware. Looking at firmware suggests some commands looking for 3-character code. For example:

	UL01B

(you may need to send it a few times) seems to trip response code `600C`. In the firmware, it can be seen that a 3-character string is converted to hex (i.e., 01B in the example becomes 0x1B).




## Firmware Notes