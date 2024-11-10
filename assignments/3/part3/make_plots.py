import matplotlib.pyplot as plt

# values pulled from the jobresults files of part1 and part2
serial_time = 0.0000471942 # seconds
parallel_time = [0.0179603158, # 2 x 2
                 0.0030495860, # 4 x 4
                 0.0963258766, # 8 x 8
                 2.3590599194, # 16x16
                 ]


strong_speedup = [serial_time / parallel_time[0],
                  serial_time / parallel_time[1],
                  serial_time / parallel_time[2],
                  serial_time / parallel_time[3]]

x = [4, 16, 64, 256]

print(f'strong_speedup: {strong_speedup}')

plt.title('Strong Speedup')
plt.xlabel('# procs')
plt.ylabel('Strong Speedup')
plt.plot(x,strong_speedup, marker='o', label='Strong Speedup')
plt.legend()
plt.savefig('strong_speedup.png')
plt.show()