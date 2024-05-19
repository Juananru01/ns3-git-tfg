import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

csv=pd.read_csv('resultados_802.11ax_5GHz.csv', sep=',')
dataframe=pd.DataFrame(csv[{"MCS","Mean_Throughput(Mbps)","Distance(m)"}])
# print(dataframe)

plt.figure(figsize=(10, 6))

for mcs, group in dataframe.groupby('MCS'):
    plt.plot(group['Distance(m)'], group['Mean_Throughput(Mbps)'], marker='o', label=f'MCS {mcs}')

plt.xlabel('Distance (m)')
plt.ylabel('Mean Throughput (Mbps)')
plt.title('Mean Throughput vs Distance - 802.11ax (5GHZ)')

plt.yticks(np.arange(dataframe['Mean_Throughput(Mbps)'].min(), dataframe['Mean_Throughput(Mbps)'].max() + 50, 50))

plt.legend()
plt.grid(True)

plt.show()