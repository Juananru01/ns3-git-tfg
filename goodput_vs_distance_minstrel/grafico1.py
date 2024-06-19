import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

csv=pd.read_csv('goodput_vs_distance_minstrel/goodput_vs_distance_802.11ax_minstrel_gi800.csv', sep=',')
dataframe=pd.DataFrame(csv[{"Mean_Goodput(Mbps)","Distance(m)","FrequencyBand(GHz)"}])

plt.figure(figsize=(8, 5))

for frequencyBand, group in dataframe.groupby('FrequencyBand(GHz)'):
    plt.plot(group['Distance(m)'], group['Mean_Goodput(Mbps)'], marker='o', label=f'{frequencyBand} GHz')

plt.xlabel('Distance(m)', fontsize=14)
plt.ylabel('Mean Goodput (Mbps)', fontsize=14)
#plt.title('Mean Goodput vs Distance - 802.11ax - Minstrel')

plt.ylim(top=200+5)
plt.xticks(np.arange(0, dataframe['Distance(m)'].max() + 5, 10), fontsize=13)
plt.yticks(np.arange(0, 201, 20), fontsize=13)

plt.legend(title='Frequency Band', fontsize=12)
plt.grid(True)

plt.subplots_adjust(left=0.1, right=0.95, top=0.95, bottom=0.1)
plt.show()