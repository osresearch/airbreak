![CPAP running custom firmware](images/ventilator-animated.gif)
# Airbreak

The Airsense 10 CPAP (Constant Positive Airway Pressure) machine
is a common, low-cost sleep therapy device intended to treat sleep apnea and
other respiratory disorders.  

In light of the COVID-19 crisis and resultant shortage of medical equipment, [hospitals have been using BiPAP (BIlevel Positive Airway Pressure) machines as non-invasive ventilators](https://health.mountsinai.org/blog/mount-sinai-turns-hundreds-of-machines-for-sleep-apnea-into-hospital-ventilators-shares-instructions-worldwide/), and [several groups](https://github.com/PubInv/covid19-vent-list) are currently working on ventilator designs that involve a converted BiPAP. CPAP devices are cheaper, more widely available, and similar electrically and mechanically. However, they are not considered useful; 
according to their manufacturer, CPAP machines "[*would require significant rework in order to function as a ventilator*](https://www.resmed.com/en-us/covid-19/)".

What we have done it to "*jailbreak*" the CPAP machine so that it
is possible to run additional tasks on the device to add the features
necessary to use the device as a temporary ventilator.  This can help ease
the shortage until more real ventilators are available.

Our changes bring the Airsense S10 to near feature parity with BiPAP machines from the same manufacturer, boost the maximum pressure output available, and provide a starting point to add more advanced emergency ventilator functionality.

## Disclaimer

While we are consulting with doctors to validate that the modified firmware works as intended, the codebase in its current form should be considered
a proof of concept and is not intended for use in a life-support capacity.  

No IP belonging to the device manufacturer is hosted in this repository, nor will it be accepted as a pull request.

## Major features

* Closed loop air volume control with backup respiration rates
* Allows maximum pressure to be increased to 30 cm H2O
* Adds a timed breathing mode that oscillates between high and low pressure
* Allows very rapid pressure change rates compared to the stock slow ramp 0.2 cm/sec
* Reads tuning parameters from a memory location accessible over SWD
* Unlocks all of the vendor modes and tunable configuration parameters
* Access to all of the sensors (flow, pressure, temperature, etc)

## Planned features
* Draws graphs on the screen to show an immediate history of data (almost ready)
* Visual and audible alarms when flow stoppage or leakage rates are detected
* GPIO interface with other systems


## Evaluation

![PV curves](images/pv.png)

To be written.  Currently the modified firmware is being evaluated by
a research lab.  There are other pieces necessary to use the devices in
a clinical setting, such as viral traps and O2 inputs.

[Mt Sinai's BiPAP protocols for non-invasive ventilation](https://health.mountsinai.org/wp-content/uploads/sites/14/2020/04/NIV-to-Ventilator-Modification-Protocol-v1.02-for-posting.pdf)
provide a roadmap for how these CPAP devices could be used.

### More details


* [Installation guide](/disassembly)
* [Writing extensions](/info/extensions.md)
