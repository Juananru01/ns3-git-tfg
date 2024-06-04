import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

csv=pd.read_csv('goodput_vs_mcs_802.11ax_2.4GHz_gi3200.csv', sep=',')
dataframe=pd.DataFrame(csv[{"MCS","Mean_Goodput(Mbps)","ChannelWidth(Hz)"}])

plt.figure(figsize=(10, 7))

for channelWidth, group in dataframe.groupby('ChannelWidth(Hz)'):
    plt.plot(group['MCS'], group['Mean_Goodput(Mbps)'], marker='o', label=f'{channelWidth} Hz')

plt.xlabel('MCS')
plt.ylabel('Mean Goodput (Mbps)')
plt.title('Mean Goodput vs MCS - 802.11ax (2.4GHz) - 3200 GI')

plt.ylim(top=280+5)
plt.xticks(np.arange(0, dataframe['MCS'].max() + 1, 1))
plt.yticks(np.arange(0, 281, 20))

plt.legend(title='Channel Width', loc='upper left')
plt.grid(True)

plt.show()