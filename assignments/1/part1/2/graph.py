import pandas as pd
import numpy as np
from matplotlib import pyplot as plt

plt.rcParams["figure.figsize"] = [7.00, 3.50]
plt.rcParams["figure.autolayout"] = True
columns = ["Time", "Message_size"]
df = pd.read_csv("job.o2519338", usecols=columns)

thelist = []
for i in range(0, 30):
    thelist.append(df['Time'][i::30].mean())

df['Msize'] = df['Message_size'][0:30]

sizes = df['Message_size'][0:30].to_numpy()

print(sizes)
list2 = []

for i in range(28,30):
    list = [1, sizes[i]*8]
    list2.append(list)

x = np.linalg.solve(list2, thelist[28:30])

print(x)

print(f'sizes2 {list2}')
for i in range(len(thelist), len(df["Time"])):
    thelist.append(None)

df["Average"] = thelist

print("Contents in csv file:\n", df)



plt.xscale("log")
plt.title("Tcomm vs m")
plt.ylabel("Time (seconds)")
plt.xlabel("Message size (bytes)")
plt.plot(df.Msize, df.Average)
plt.show()