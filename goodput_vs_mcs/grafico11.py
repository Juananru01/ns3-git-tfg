import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

csv=pd.read_csv('goodput_vs_mcs_802.11be_6GHz_gi800.csv', sep=',')
dataframe=pd.DataFrame(csv[{"MCS","Mean_Goodput(Mbps)","ChannelWidth(Hz)"}])

plt.figure(figsize=(10, 7))

for channelWidth, group in dataframe.groupby('ChannelWidth(Hz)'):
    plt.plot(group['MCS'], group['Mean_Goodput(Mbps)'], marker='o', label=f'{channelWidth} Hz')

plt.xlabel('MCS')
plt.ylabel('Mean Goodput (Mbps)')
plt.title('Mean Goodput vs MCS - 802.11be (6GHz) - 800 GI')

plt.ylim(top=800+10)
plt.xticks(np.arange(0, dataframe['MCS'].max() + 1, 1))
plt.yticks(np.arange(0, 801, 50))

plt.legend(title='Channel Width')
plt.grid(True)

plt.show()