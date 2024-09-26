import pandas as pd
import numpy as np
from matplotlib import pyplot as plt

plt.rcParams["figure.figsize"] = [7.00, 3.50]
plt.rcParams["figure.autolayout"] = True
columns = ["Time", "Message_size"]
print('reading job.othird')
df = pd.read_csv("job.othird", usecols=columns)

end = 12


thelist = []
for i in range(0, end):
    thelist.append(df['Time'][i::end].mean())

laterlist = thelist

for i in range(len(thelist), len(df["Time"])):
    thelist.append(None)

df["Average"] = thelist

# graph config
plt.xscale("log")
plt.title("Tcomm vs m")
plt.ylabel("Time (seconds)")
plt.xlabel("Message size (bytes)")

# graph the scatter plot
plt.scatter(df['Message_size'][0:end], df['Average'][0:end])

# get line of best fit and plot it
a, b = np.polyfit(df['Message_size'][0:end].to_numpy(), laterlist[0:end], 1)
plt.plot(df['Message_size'][0:end], a * df['Message_size'][0:end].to_numpy() + b)

# print out our calculated tau, mu, and transfer rate
print(f"MU={a} TAU={b} TRANSFER_RATE={1/a} Bytes ({1/a/1073741824} GiB/s) ({1/a/1073741824*8} Gib/s)")

for xy in zip(df['Message_size'][0:end].to_numpy(), laterlist[0:end]):
    plt.annotate('(%s,%05.5s)' % xy, xy=xy, textcoords='data')

plt.show()