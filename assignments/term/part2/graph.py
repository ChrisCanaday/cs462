import matplotlib.pyplot as plt

comm_times = [0.0116205487, #  1x1x1
              0.0345491125, #  2x2x2
              0.0389265239, #  4x4x4
              0.8548686944, #  8x8x8
              ]

comp_times = [9.0052927778,  # 1x1x1
              1.2183946498,  # 2x2x2
              0.0684560120,  # 4x4x4
              0.0086095084,  # 8x8x8
              ]
total_times = [9.0169133265,  #  1x1x1
               1.2529437623,  #  2x2x2
               0.1073825359,  #  4x4x4
               0.8634782028,  #  8x8x8
               ]

speedup = [total_times[0] / total_times[0],
           total_times[0] / total_times[1],
           total_times[0] / total_times[2],
           total_times[0] / total_times[3],
           ]

x = [1, 8, 64, 512]

print(f'speedup: {speedup}')

# comp time
fig, ax = plt.subplots()
ax.set_xscale('log', base=2)
plt.title('Computation Time')
plt.xlabel('# procs')
plt.ylabel('Seconds')
ax.plot(x, comp_times, marker='o')
plt.savefig('graphs/comp.png')
#plt.show()
plt.clf()

# comm time
fig, ax = plt.subplots()
ax.set_xscale('log', base=2)
plt.title('Communication Time')
plt.xlabel('# procs')
plt.ylabel('Seconds')
ax.plot(x, comm_times, marker='o')
plt.savefig('graphs/comm.png')
#plt.show()
plt.clf()

# total time
fig, ax = plt.subplots()
ax.set_xscale('log', base=2)
plt.title('Total Time')
plt.xlabel('# procs')
plt.ylabel('Seconds')
ax.plot(x, total_times, marker='o')
plt.savefig('graphs/total.png')
#plt.show()
plt.clf()

# Speedup
fig, ax = plt.subplots()
ax.set_xscale('log', base=2)
plt.title('Speedup')
plt.xlabel('# procs')
plt.ylabel('Speedup')
ax.plot(x,speedup, marker='o', label='Speedup')
plt.legend()
plt.savefig('graphs/speedup.png')
#plt.show()
plt.clf()