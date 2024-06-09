import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

csv=pd.read_csv('goodput_vs_mcs/goodput_vs_mcs_802.11ax_2.4GHz_gi800.csv', sep=',')
csv2=pd.read_csv('goodput_vs_mcs/goodput_vs_mcs_802.11ax_2.4GHz_gi3200.csv', sep=',')

dataframe=pd.DataFrame(csv[{"MCS","Mean_Goodput(Mbps)","ChannelWidth(Hz)"}])
dataframe2=pd.DataFrame(csv2[{"MCS","Mean_Goodput(Mbps)","ChannelWidth(Hz)"}])

plt.figure(figsize=(10, 7))

colors = {
    20: 'green',
    40: 'red'
}

for channelWidth, group in dataframe.groupby('ChannelWidth(Hz)'):
    color = colors.get(channelWidth, None)
    plt.plot(group['MCS'], group['Mean_Goodput(Mbps)'], marker='o', label=f'{channelWidth} Hz - 800 ns', color = color)

for channelWidth, group in dataframe2.groupby('ChannelWidth(Hz)'):
    color = colors.get(channelWidth, None)
    plt.plot(group['MCS'], group['Mean_Goodput(Mbps)'], marker='o', label=f'{channelWidth} Hz - 3200 ns', linestyle=":", color = color)

plt.xlabel('MCS')
plt.ylabel('Mean Goodput (Mbps)')
plt.title('Mean Goodput vs MCS - 802.11ax (2.4GHz)')

plt.ylim(top=280+5)
plt.xticks(np.arange(0, dataframe['MCS'].max() + 1, 1))
plt.yticks(np.arange(0, 281, 20))

plt.legend(title='Channel Width - GI', loc='upper left')
plt.grid(True)

plt.show()