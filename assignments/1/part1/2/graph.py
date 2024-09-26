import pandas as pd
import numpy as np
from matplotlib import pyplot as plt

plt.rcParams["figure.figsize"] = [7.00, 3.50]
plt.rcParams["figure.autolayout"] = True
columns = ["Time", "Message_size"]
df = pd.read_csv("job.osecond", usecols=columns)

end = 11


thelist = []
for i in range(0, end):
    thelist.append(df['Time'][i::end].mean())

df['Msize'] = df['Message_size'][0:end]

sizes = df['Message_size'][0:end].to_numpy()

print(sizes)
list2 = []

laterlist = thelist

for i in range(end-2,end):
    list = [1, sizes[i]*8]
    list2.append(list)

x = np.linalg.solve(list2, thelist[end-2:end])

print(x)

print(f'sizes2 {list2}')
for i in range(len(thelist), len(df["Time"])):
    thelist.append(None)

df["Average"] = thelist

print("Contents in csv file:\n", df)



#plt.xscale("log")
plt.title("Tcomm vs m")
plt.ylabel("Time (seconds)")
plt.xlabel("Message size (bytes)")
plt.scatter(df.Msize, df.Average)

print(f"len1: {len(df['Message_size'][0:end].to_numpy())}")
print(f'len2: {len(laterlist[0:end])}')

a, b = np.polyfit(df['Message_size'][0:end].to_numpy(), laterlist[0:end], 1)
plt.plot(df['Message_size'][0:end], a * df['Message_size'][0:end].to_numpy() + b)

print(f"a={a} b={b} 1/b={1/b}")


#plt.plot(df.Msize, df.Average)
plt.show()