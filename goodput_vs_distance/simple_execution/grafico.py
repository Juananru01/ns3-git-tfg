import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

csv=pd.read_csv('goodput_vs_distance/resultados_802.11ax_v2_5GHz_channelWidth160MHz_gi800.csv', sep=',')
dataframe=pd.DataFrame(csv[{"MCS","Mean_Throughput(Mbps)","Distance(m)"}])
# print(dataframe)

filtered_dataframe = dataframe[dataframe['Distance(m)'] <= 70]

plt.figure(figsize=(10, 6))

colors = {
    10: 'orange',
    11: 'brown'
}

for mcs, group in filtered_dataframe.groupby('MCS'):
    color = colors.get(mcs, None)
    plt.plot(group['Distance(m)'], group['Mean_Throughput(Mbps)'], marker='o', label=f'MCS {mcs}', color=color)

plt.xlabel('Distance (m)', fontsize=18)
plt.ylabel('Mean Goodput (Mbps)', fontsize=18)
#plt.title('Mean Goodput vs Distance - 802.11ax (5GHz)')

plt.ylim(top=800+10)
plt.yticks(np.arange(filtered_dataframe['Mean_Throughput(Mbps)'].min(), 801, 50), fontsize=17)
plt.xticks(fontsize=17)

plt.legend(fontsize=16)
plt.grid(True)

plt.subplots_adjust(left=0.1, right=0.95, top=0.95, bottom=0.1)
plt.show()