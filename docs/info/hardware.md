# Airsense S10 Hardware

This page aggregates hardware details about the Airsense S10 electronics
and other information that may be relevant to the reverse engineering process.

## FCC Filings

One crucial reverse engineering resource is the
[FCC documentation for the AirSense S10](https://fccid.io/2ACHL-AIR104G), which
includes external and internal schematics and user manuals.

[The FCC filings for the AirCurve BiPAP](https://fccid.io/2ACHL-A10STA3G),
currently in use as an emergency ventilator, shows very similar electronics.

## Clinical Manuals

[Clinical manuals](https://www.manualslib.com/brand/resmed/medical-equipment.html)
for the assisted-breathing devices from this manufacturer provide a great deal
of insight into the various models in the AirSet and AirCurve lines, as well as
the ventilation features available in each one.
Many of the menu options restricted to other models can be enabled by flipping
configuration bits in the firmware.

## Components 

Here are some of the components on the AirSense S10 

Main PCB

| Part  		| Part No. | Manufacturer | Reference | Additional |
| ---			| --- | --- | --- | --- |
| Main uC		| STM32F405ZGT6 (LQFP144) | STMicroelectronics | [Datasheet](https://www.st.com/resource/en/datasheet/stm32f405zg.pdf) | [Reference Manual](https://www.st.com/resource/en/reference_manual/cd00190271-stm8s-series-and-stm8af-series-8bit-microcontrollers-stmicroelectronics.pdf) | 
| Aux uC		| STM8S005 (TQFP48) |  STMicroelectronics | [Datasheet](https://www.st.com/content/ccc/resource/technical/document/datasheet/21/5a/57/8b/f4/49/4a/12/DM00037613.pdf/files/DM00037613.pdf/jcr:content/translations/en.DM00037613.pdf)  | [Reference Manual](https://www.st.com/resource/en/reference_manual/cd00190271-stm8s-series-and-stm8af-series-8bit-microcontrollers-stmicroelectronics.pdf) | 
| Power watchdog IC	| POWR605  | Lattice | [Datasheet](http://www.latticesemi.com/~/media/LatticeSemi/Documents/DataSheets/ProcessorPM-POWR605DataSheet.pdf) | Monitors pressures - see notes below|
| Main Pressure Sensor  | NPA-300M-015A | Amphenol | [Datasheet](https://amphenol-sensors.com/en/component/edocman/212-novasensor-npa-surface-mount-pressure-sensors-datasheet/download?Itemid=8429) | Operates in analog mode |
| Differential Pressure Sensor | SDP8xx | Sensirion | [Datasheet](https://www.sensirion.com/en/about-us/company/technology/cmosens-technology-for-gas-flow/) | Digital mode, manifold mount |
| Programming Header	| TC2050-IDC | Tag-Connect LLC | [DigiKey](https://www.digikey.com/product-detail/en/tag-connect-llc/TC2050-IDC/TC2050-IDC-ND/2605366) | 
| Motor Driver | DRV8302 | Texas Instruments | [DigiKey](https://www.digikey.com/product-detail/en/tag-connect-llc/TC2050-IDC/TC2050-IDC-ND/2605366) | 
| Serial EEPROM | M95M02 | STMicroelectronics | [Datasheet](https://www.st.com/resource/en/datasheet/m95m02-dr.pdf) | 
| LCD | TM024HDZ29 | Tianma | [Similar datasheets](https://www.beyondinfinite.com/product/lcd/tianma_lcd_display.html) | [Closest we could find](https://z3d9b7u8.stackpathcdn.com/pdf-down/T/M/0/TM024HDH30-01-Tianma.pdf)|
| Relay Driver | A2550 | Allegro | [Datasheet](https://www.allegromicro.com/~/media/Files/Datasheets/A2550-Datasheet.ashx) | Function unknown; accessory? | 
| Logic Voltage Regulator (x2) | ISL9000A | Renesas | [Datasheet](https://www.mouser.ca/datasheet/2/698/isl9000a-1529533.pdf) | Generates 3.3v and 5v logic voltages | 
| Top Switch | B3F-3125 | Omrom | [Digikey](https://www.digikey.com/product-detail/en/omron-electronics-inc-emc-div/B3F-3125/SW851-ND/368368) | In case switch needs to be replaced (confirmed exact fit)|

Cellular Module & Antenna (CAM) Board

| Part  		| Part No. | Manufacturer | Reference | Additional |
| ---			| --- | --- | --- | --- |
| Main uC		| R5F5210ABDFP | Renesas | [Datasheet](https://www.renesas.com/eu/en/doc/products/mpumcu/doc/rx_family/r01ds0041ej0150_rx210.pdf) | LFQFP 100-pin |
| CDMA Radio Module| ce910-dual | Telit | [Datasheet](https://static6.arrow.com/aropdfconversion/1e95506f859937cecfcd3d7fd07cc25d6eab906d/telit_ce910-dual_datasheet.pdf) | [qualcomm qsc1105 chipset](https://fccid.io/RI7CE910-DUAL/Internal-Photos/Internal-Photos-1760716) |
| 3G Radio Module| HE910-D | Telit | [Datasheet](hhttps://www.telit.com/wp-content/uploads/2017/09/160229_DS_HE910.pdf) | [Uknown chipset- FCC photos obscured](https://fccid.io/RI7HE910/Internal-Photos/Internal-Photos-1654391)|
| 4G Radio Module| cinterion els61-us | Gemalto | [Datasheet](https://www.gemalto.com/brochures-site/download-site/Documents/M2M_ELS61_datasheet.pdf) | Cat 1 LTE w/ 2G/3G fallback | 

Alarm Board (Only A10STACD, A10STA3G, AIR104G)

| Part  		| Part No. | Manufacturer | Reference | Additional |
| ---			| --- | --- | --- | --- |
| Main uC		| R5F21358CNFP | Renesas | [Datasheet](https://4donline.ihs.com/images/VipMasterIC/IC/RNCC/RNCCS11762/RNCCS11762-1.pdf) | [Avnet](https://www.avnet.com/shop/emea/products/renesas-electronics/r5f21358cnfp-30-3074457345629542777/) |

## Power Watchdog

The power watchdog IC (POWR605) is a complicated power watch-dog IC. More than just a basic device, it contains:

* 6 DC power monitoring inputs.
* 16 macrocell PLD logic.
* 4 timers.

The 6 inputs have programmable thresholds (despite what the datasheet implies about fixed).

This device is important as appears to be used as a "safety" on pressure sensors. If pressures are too far out-of-spec it will reset the main STM32F407ZG microcontroller. More reverse engineering is needed to understand the exact connection here.

The POWR605 logic should be reprogrammable via the tag-connect header. The following works (but may not be easiest/lowest-cost):

* "Lattice Diamond" programming software.
* Programming Cable supported by above (such as HW-USBN-2B).
* Lattice "PAC-Designer" software.

To read the device, ensure you configure the Lattice Diamond SW to perform a "JEDEC Readback" - the default operation is erase/program/verify. This will then generate a `.jed` file.

From PAC-Designer, create a new project with the POWR605. You can then use the `Import` option to open the `.jed` to see configuration.
