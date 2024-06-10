import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

csv=pd.read_csv('goodput_vs_distance_802.11be_minstrel_maxWidthChannels_gi800.csv', sep=',')
dataframe=pd.DataFrame(csv[{"Mean_Goodput(Mbps)","Distance(m)","FrequencyBand(GHz)"}])

plt.figure(figsize=(10, 7))

for frequencyBand, group in dataframe.groupby('FrequencyBand(GHz)'):
    plt.plot(group['Distance(m)'], group['Mean_Goodput(Mbps)'], marker='o', label=f'{frequencyBand} GHz')

plt.xlabel('Distance(m)')
plt.ylabel('Mean Goodput (Mbps)')
plt.title('Mean Goodput vs Distance - 802.11be - Minstrel')

plt.ylim(top=525+5)
plt.xticks(np.arange(0, dataframe['Distance(m)'].max() + 5, 10))
plt.yticks(np.arange(0, dataframe['Mean_Goodput(Mbps)'].max() + 50, 25))

plt.legend(title='Frequency Band')
plt.grid(True)

plt.show()