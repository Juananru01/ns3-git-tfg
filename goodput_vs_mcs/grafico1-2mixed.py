import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

csv=pd.read_csv('goodput_vs_mcs/goodput_vs_mcs_802.11ax_5GHz_gi800.csv', sep=',')
csv2=pd.read_csv('goodput_vs_mcs/goodput_vs_mcs_802.11ax_5GHz_gi3200.csv', sep=',')

dataframe=pd.DataFrame(csv[{"MCS","Mean_Goodput(Mbps)","ChannelWidth(Hz)"}])
dataframe2=pd.DataFrame(csv2[{"MCS","Mean_Goodput(Mbps)","ChannelWidth(Hz)"}])

plt.figure(figsize=(10, 7))

colors = {
    20: 'green',
    40: 'blue',
    80: 'purple',
    160: 'red'
}

for channelWidth, group in dataframe.groupby('ChannelWidth(Hz)'):
    color = colors.get(channelWidth, None)
    plt.plot(group['MCS'], group['Mean_Goodput(Mbps)'], marker='o', label=f'{channelWidth} Hz - 800 ns', color = color)

for channelWidth, group in dataframe2.groupby('ChannelWidth(Hz)'):
    color = colors.get(channelWidth, None)
    plt.plot(group['MCS'], group['Mean_Goodput(Mbps)'], marker='o', label=f'{channelWidth} Hz - 3200 ns', linestyle=":", color = color)

plt.xlabel('MCS', fontsize=14)
plt.ylabel('Mean Goodput (Mbps)', fontsize=14)
#plt.title('Mean Goodput vs MCS - 802.11ax (5GHz)')

plt.ylim(top=800+10)
plt.xticks(np.arange(0, dataframe['MCS'].max() + 1, 1), fontsize=13)
plt.yticks(np.arange(0, 801, 50), fontsize=13)

plt.legend(title='Channel Width - GI', loc='upper left', fontsize=12)
plt.grid(True)

plt.subplots_adjust(left=0.1, right=0.95, top=0.95, bottom=0.1)
plt.show()