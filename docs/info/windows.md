# Windows Instructions

*WARNING:* Be aware that the STM32F series has a 'code read protection' feature. The only legitimate way to 'disable' this is to perform a chip erase. Current devices investigated do not have this enabled - but if this feature was enabled, you will receive an error message that may say something simple such as "Do you want to disable read protection?". If you perform this action, you will have erased the firmware.

## Programming with ST-Link Utility

If using a legitimate or fake ST-Link/V2, you will find it most convient to use the ST-Link utility for basic operation (read/write).

To do this, download the ST-Link utility at: https://www.st.com/en/development-tools/stsw-link004.html. Scroll down the page until the "Get Software" button is visible.

## Reading

You must use the "Command Line Interface" to read flash (the option is not in the ST-Link GUI as of the latest version). Open a command prompt on Windows, and run the following two commands. The first one does the read, the second one verifies it:

	ST-LINK_CLI.exe -c -dump 0x08000000 0x100000 stm32.bin
	ST-LINK_CLI.exe -CmpFile stm32.bin 0x8000000

## Writing

You can either use the GUI to do the writing, or run the following command:

	ST-LINK_CLI.exe -ME -P stm32-unlocked.bin 0x08000000 -V