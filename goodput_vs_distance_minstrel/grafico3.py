import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

csv=pd.read_csv('goodput_vs_distance_minstrel/goodput_vs_distance_802.11be_minstrel_gi800.csv', sep=',')
dataframe=pd.DataFrame(csv[{"Mean_Goodput(Mbps)","Distance(m)","FrequencyBand(GHz)"}])

plt.figure(figsize=(8, 5))

for frequencyBand, group in dataframe.groupby('FrequencyBand(GHz)'):
    plt.plot(group['Distance(m)'], group['Mean_Goodput(Mbps)'], marker='o', label=f'{frequencyBand} GHz')

plt.xlabel('Distance(m)', fontsize=18)
plt.ylabel('Mean Goodput (Mbps)', fontsize=18)
#plt.title('Mean Goodput vs Distance - 802.11be - Minstrel')

plt.ylim(top=200+5)
plt.xticks(np.arange(0, dataframe['Distance(m)'].max() + 5, 10), fontsize=17)
plt.yticks(np.arange(0, 201, 20), fontsize=17)

plt.legend(title='Frequency Band', title_fontsize=16, fontsize=15)
plt.grid(True)

plt.subplots_adjust(left=0.15, right=0.90, top=0.90, bottom=0.15)
plt.show()