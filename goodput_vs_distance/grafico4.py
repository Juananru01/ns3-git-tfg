import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

csv=pd.read_csv('goodput_vs_distance/resultados_802.11be_2.4GHz_channelWidth40MHz_gi800.csv', sep=',')
dataframe=pd.DataFrame(csv[{"MCS","Mean_Throughput(Mbps)","Distance(m)"}])
# print(dataframe)

plt.figure(figsize=(10, 6))

colors = {
    10: 'orange',
    11: 'brown',
    12: 'gold',
    13: 'purple'
}

for mcs, group in dataframe.groupby('MCS'):
    color = colors.get(mcs, None)
    plt.plot(group['Distance(m)'], group['Mean_Throughput(Mbps)'], marker='o', label=f'MCS {mcs}', color=color)

plt.xlabel('Distance (m)', fontsize=16)
plt.ylabel('Mean Goodput (Mbps)', fontsize=16)
#plt.title('Mean Goodput vs Distance - 802.11be (2.4GHz)')

plt.ylim(top=275+5)
plt.xticks(np.arange(dataframe['Distance(m)'].min(), 101, 10), fontsize=15)
plt.yticks(np.arange(dataframe['Mean_Throughput(Mbps)'].min(), 276, 25), fontsize=15)

plt.legend(fontsize=14)
plt.grid(True)

plt.subplots_adjust(left=0.1, right=0.95, top=0.95, bottom=0.1)
plt.show()