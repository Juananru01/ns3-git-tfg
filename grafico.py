import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

min_distance = 0
max_distance = 70
margin = 3

csv=pd.read_csv('resultados_802.11ax_v2_5GHz_channelWidth160MHz_gi800.csv', sep=',')
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

plt.xlabel('Distance (m)')
plt.ylabel('Mean Goodput (Mbps)')
plt.title('Mean Goodput vs Distance - 802.11ax (5GHZ)')

plt.xlim(min_distance - margin, max_distance + margin)
plt.yticks(np.arange(filtered_dataframe['Mean_Throughput(Mbps)'].min(), filtered_dataframe['Mean_Throughput(Mbps)'].max() + 50, 50))

plt.legend()
plt.grid(True)

plt.show()