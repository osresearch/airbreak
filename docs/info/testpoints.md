# Useful Test Points

The Airsense S10 mainboard is covered in test points. Here are some
that may be of interest.


|  Pin | Description |
| --- | --- |
| <span style="color:#ff0000">STM32_3V3</span> | Core voltage for main uC. |
| GND | Ground. |
| <span style="color:#0000ff">SWDIO</span> | Used to program and debug main uC. |
| <span style="color:#656565">SWCLK</span> | Used to program and debug main uC. |
| <span style="color:#4f0099">SWO</span> | Could potentially be used to get further debug output from main uC (TBD). |
| <span style="color:#ff7f00">STM32_NRST</span> | Reset pin for main uC. |
| <span style="color:#66ff66">PSENSOR</span> | Scope this to get an analog pressure readout without external equipment. |
| <span style="color:#00ffff">STM8_UART</span> | UART_TX from STM8 aux uC to main uC |
| <span style="color:#663300">STM8_3V3</span> | Core voltage for aux uC |
| <span style="color:#ff00ff">STM8_SWIM</span> | Single-wire programming and debug pin for aux uC |

[![Photo of test points on the Airsense PCB](/images/testpoints.jpg)](https://airbreak.dev/images/testpoints.jpg)
