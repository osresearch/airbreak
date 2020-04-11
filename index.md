![CPAP running custom firmware](images/airsense-hacked.jpg)
# Jailbreak CPAP machines to make temporary ventilators

The Airsense 10 CPAP machine is a low cost sleep therapy device that
provides a constant air pressure to help with sleep apnea and other disorders.
While [hospitals have been using BiPAP machines as non-invasive ventilators](https://health.mountsinai.org/blog/mount-sinai-turns-hundreds-of-machines-for-sleep-apnea-into-hospital-ventilators-shares-instructions-worldwide/),
the more widely available and lower costs CPAP devices were not considered usable
and the manufacturer says their CPAP machines "[*would require significant rework in order to function as a ventilator*](https://www.resmed.com/en-us/covid-19/)".

What we have done it to "*jailbreak*" the CPAP machine so that it
is possible to run additional tasks on the device to add the features
necessary to use the device as a temporary ventilator.  This can help ease
the shortage until more real ventilators are available.  This process
also unlocks all of the modes and configuration parameters available in
the vendor firmware.

## Major features:
* Closed loop air volume control with backup respiration rates
* Allows maximum pressure to be increased to 30 cm H2O
* Adds a timed breathing mode that oscillates between high and low pressure
* Allows very rapid pressure change rates compared to the stock slow ramp 0.2 cm/sec
* Reads tuning parameters from a memory location accessible over SWD
* Unlocks all of the vendor modes and tunable configuration parameters
* Access to all of the sensors (flow, pressure, temperature, etc)

## Possible new features:
* Draws graphs on the screen to show an immediate history of data (almost ready)
* Visual and audible alarms when flow stoppage or leakage rates are detected
* GPIO interface with other systems


# Evaluation

![PV curves](images/pv.png)

To be written.  Currently the modified firmware is being evaluated by
a research lab.  There are other pieces necessary to use the devices in
a clinical setting, such as viral traps and O2 inputs.

[Mt Sinai's BiPAP protocols for non-invasive ventilation](https://health.mountsinai.org/wp-content/uploads/sites/14/2020/04/NIV-to-Ventilator-Modification-Protocol-v1.02-for-posting.pdf)
provide a roadmap for how these CPAP devices could be used.

# More details

* [Installation instructions](Installation.md)
* [Writing extensions](Extensions.md)
