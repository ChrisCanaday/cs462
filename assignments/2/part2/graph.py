import pandas as pd
import numpy as np
from matplotlib import pyplot as plt

procs   = [1, 2, 4, 8, 16, 32]
n       = [3, 4, 5, 6, 7]
nsteps  = [10**3, 10**4, 10**5, 10**6, 10**7]
p1_data = [0.10944299, 0.07546670, 0.03546286, 0.01707253, 0.02010407, 0.00959684]
p2_data = [0.00564180, 0.00325001, 0.00635564, 0.00045873, 0.02140428]


fig, axs = plt.subplots(2)

fig.suptitle('Numerical Integration')

axs[0].plot(procs, p1_data)
axs[0].set(xlabel='Number of Processes', ylabel='Execution Time (seconds)')
axs[1].plot(nsteps, p2_data)
axs[1].set(xlabel='Number of Steps', ylabel='Execution Time (seconds)')

fig.tight_layout()
plt.show()