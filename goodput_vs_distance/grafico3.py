import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

csv=pd.read_csv('resultados_802.11ax_v2_6GHz_channelWidth160MHz_gi800.csv', sep=',')
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
plt.title('Mean Goodput vs Distance - 802.11ax (6GHz)')

plt.ylim(top=800+10)
plt.yticks(np.arange(filtered_dataframe['Mean_Throughput(Mbps)'].min(), 801, 50))

plt.legend()
plt.grid(True)

plt.show()