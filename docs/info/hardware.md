# Airsense S10 Hardware

This page aggregates hardware details about the Airsense S10 electronics
and other information that may be relevant to the reverse engineering process.

## FCC Filings

One crucial reverse engineering resource is the
[FCC documentation for the AirSense S10](https://fccid.io/2ACHL-AIR104G), which
includes external and internal schematics and user manuals.

[The FCC filings for the AirCurve BiPAP](https://fccid.io/2ACHL-A10STA3G),
currently in use as an emergency ventilator, shows very similar electronics.

## Components 

Here are some of the components on the AirSense S10 main PCB.

| Part  		| Part No. | Manufacturer | Reference | Additional |
| ---			| --- | --- | --- | --- |
| Main uC		| STM32F405ZGT6 (LQFP144) | STMicroelectronics | [Datasheet](https://www.st.com/resource/en/datasheet/stm32f405zg.pdf) | [Reference Manual](https://www.st.com/resource/en/reference_manual/cd00190271-stm8s-series-and-stm8af-series-8bit-microcontrollers-stmicroelectronics.pdf) | 
| Aux uC		| STM8S005 (TQFP48) |  STMicroelectronics | [Datasheet](https://www.st.com/content/ccc/resource/technical/document/datasheet/21/5a/57/8b/f4/49/4a/12/DM00037613.pdf/files/DM00037613.pdf/jcr:content/translations/en.DM00037613.pdf)  | [Reference Manual](https://www.st.com/resource/en/reference_manual/cd00190271-stm8s-series-and-stm8af-series-8bit-microcontrollers-stmicroelectronics.pdf)   | 
| Power watchdog IC	| POWR605  | Lattice | [Datasheet](http://www.latticesemi.com/~/media/LatticeSemi/Documents/DataSheets/ProcessorPM-POWR605DataSheet.pdf) | Monitors pressures - see notes below.
| Main Pressure Sensor  | NPA-300M-015A | Amphenol | [Datasheet](https://amphenol-sensors.com/en/component/edocman/212-novasensor-npa-surface-mount-pressure-sensors-datasheet/download?Itemid=8429) | Operates in analog mode |
| Differential Pressure Sensor | SDP8xx | Sensirion | [Datasheet](https://www.sensirion.com/en/about-us/company/technology/cmosens-technology-for-gas-flow/) | Digital mode, manifold mount |
| Programming Header	| TC2050-IDC | Tag-Connect LLC | [DigiKey](https://www.digikey.com/product-detail/en/tag-connect-llc/TC2050-IDC/TC2050-IDC-ND/2605366) | 
| Motor Driver | DRV8302 | Texas Instruments | [DigiKey](https://www.digikey.com/product-detail/en/tag-connect-llc/TC2050-IDC/TC2050-IDC-ND/2605366) | 
| Serial EEPROM | M95M02 | STMicroelectronics | [Datasheet](https://www.st.com/resource/en/datasheet/m95m02-dr.pdf) | 
| LCD | TM024HDZ29 | Tianma | [Similar datasheets](https://www.beyondinfinite.com/product/lcd/tianma_lcd_display.html) | [Closest we could find](https://www.beyondinfinite.com/lcd/Library/Tianma/TM024HDZ73.pdf)
| Relay Driver | A2550 | Allegro | [Datasheet](https://www.allegromicro.com/~/media/Files/Datasheets/A2550-Datasheet.ashx) | Function unknown; accessory? | 
| Top Switch | B3F-3125 | Omrom | [Digikey](https://www.digikey.com/product-detail/en/omron-electronics-inc-emc-div/B3F-3125/SW851-ND/368368) | In case switch needs to be replaced (confirmed exact fit).

## Power Watchdog

The power watchdog IC (POWR605) is a complicated power watch-dog IC. More than just a basic device, it contains:

* 6 DC power monitoring inputs.
* 16 macrocell PLD logic.
* 4 timers.

The 6 inputs have fixed thresholds (not programmable) as follows:

* VMON1 = 3.3V
* VMON2 = 2.5V
* VMON3 = 1.8V
* VMON4 = 0.669V
* VMON5 = 0.669V
* VMON6 = 0.669V

This device is important as appears to be used as a "safety" on pressure sensors. If pressures are too far out-of-spec it will reset the main STM32F407ZG microcontroller. More reverse engineering is needed to understand the exact connection here.

The POWR605 logic should be reprogrammable via the tag-connect header.