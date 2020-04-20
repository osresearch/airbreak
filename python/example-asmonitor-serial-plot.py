import matplotlib.pylab as plt
import numpy as np
from asmonitor import ASMonitor, ASMonitor_Serial
import time

print("Connecting to low-level...")
ass = ASMonitor_Serial()
ass.connect("COM57")

asm = ASMonitor(ass)

samples = 1000

floatarrays = np.zeros( (samples, 5))

print("Starting to sample now - ensure airflow on and unit is working")
# Dump 5 variables from table - serial is way slower! But OK if you limit variables you monitor to specific
# ones only. Once you know exact address don't need to dump whole sections of tables.
for i in range(0, samples):
    floatarrays[i,0] = asm.return_therapy_float(0x2D)
    floatarrays[i,1] = asm.return_therapy_float(0x44)
    floatarrays[i,2] = asm.return_therapy_float(0x46)
    floatarrays[i,3] = asm.return_therapy_float(0x55)
    floatarrays[i,4] = asm.return_therapy_float(0x8F)
    
plt.plot(floatarrays)
plt.show()

print("Printing mode... try starting/stopping therapy and changing mode (clinical menu)")
for i in range(0, 20):
    print("Mode: %x"%asm.return_therapy_int(0x6F))
    time.sleep(1)