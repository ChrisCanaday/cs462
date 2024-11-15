import matplotlib.pyplot as plt

comm_times = [0.0051340630, #  1x1
              0.0329559657, #  4x4
              0.0759102836, #  8x8
              0.2127053688, # 16x16
              ]

comp_times = [10.8500654894, #  1x1
              0.2755965284,  #  4x4
              0.0690001433,  #  8x8
              0.017414222,   # 16x16
              ]
total_times = [10.8551995524, #  1x1
               0.3085524941,  #  4x4
               0.1449104269,  #  8x8
               0.2301195912,  # 16x16
               ]

speedup = [total_times[0] / total_times[0],
           total_times[0] / total_times[1],
           total_times[0] / total_times[2],
           total_times[0] / total_times[3],
           ]

x = [1, 16, 64, 256]

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