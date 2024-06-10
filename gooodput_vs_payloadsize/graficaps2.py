import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

csv=pd.read_csv('gooodput_vs_payloadsize/minstrel-and-mtu/goodput_vs_payloadsize_802.11be_maxChannelWidths_gi800.csv', sep=',')

dataframe=pd.DataFrame(csv[{"Mean_Goodput(Mbps)","PayloadSize(bytes)","FrequencyBand(GHz)"}])

plt.figure(figsize=(12, 7))

colors = {
    2.4: 'green',
    5: 'blue',
    6: 'red'
}

linestyles = {
    2.4: 'solid',
    5: 'solid',
    6: '--'
}

markers = {
    2.4: 'o',
    5: 'o',
    6: 'o'
}

markersizes = {
    2.4: 6,
    5: 7,
    6: 3
}

dashes = {
    2.4: [20,0],
    5: [0,1,0],
    6: [3,0,3]
}

for frequencyBand, group in dataframe.groupby('FrequencyBand(GHz)'):
    color = colors.get(frequencyBand, None)
    linestyle = linestyles.get(frequencyBand, None)
    marker = markers.get(frequencyBand, None)
    markersize = markersizes.get(frequencyBand, None)
    dashe = dashes.get(frequencyBand, None)
    line = plt.plot(group['PayloadSize(bytes)'], group['Mean_Goodput(Mbps)'], marker=marker, markersize=markersize, label=f'{frequencyBand} GHz', color=color, linestyle=linestyle, dashes=(dashe))

plt.xlabel('PayloadSize(bytes)')
plt.ylabel('Mean Goodput (Mbps)')
plt.title('Mean Goodput vs Payload Size - 802.11be - Minstrel')

plt.ylim(top=600+5)
plt.xticks(np.arange(0, dataframe['PayloadSize(bytes)'].max() + 5, 1000))
plt.yticks(np.arange(0, 601, 25))

plt.legend(title='Frequency Band',loc='lower right')
plt.grid(True)

plt.show()