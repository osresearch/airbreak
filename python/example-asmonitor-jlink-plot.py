import matplotlib.pylab as plt
import numpy as np
from asmonitor import ASMonitor, ASMonitor_JLink

print("Connecting to low-level...")
asjl = ASMonitor_JLink()
asjl.connect()

asm = ASMonitor(asjl)

samples = 2000

floatarrays = np.zeros( (samples, 255))

print("Starting to sample now - ensure airflow on and unit is working")
#Dump 255 variables from table (how many are there?)
for i in range(0, samples):
    floatarrays[i,:] = asm.return_therapy_float(num_fps=255)
    
plt.plot(floatarrays)
plt.show()