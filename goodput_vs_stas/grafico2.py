import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

csv_files = ['goodput_vs_stas_802.11be_minstrel_maxWidthChannels_gi800_seed40.csv', 
             'goodput_vs_stas_802.11be_minstrel_maxWidthChannels_gi800_seed41.csv',
             'goodput_vs_stas_802.11be_minstrel_maxWidthChannels_gi800_seed42.csv']

dataframes = [pd.read_csv(file, sep=',') for file in csv_files]

mean_df = pd.DataFrame()

for df in dataframes:
    if mean_df.empty:
        mean_df = df.copy()
        mean_df['Mean_Goodput(Mbps)'] = 0
    else:
        if not df['NumSTAs'].equals(mean_df['NumSTAs']) or not df['FrequencyBand(Hz)'].equals(mean_df['FrequencyBand(Hz)']):
            raise ValueError("Las filas de los archivos CSV no coinciden exactamente.")

for df in dataframes:
    mean_df['Mean_Goodput(Mbps)'] += df['Mean_Goodput(Mbps)']

mean_df['Mean_Goodput(Mbps)'] /= len(dataframes)

if 'Seed' in mean_df.columns:
    mean_df = mean_df.drop(columns=['Seed'])

filtered_df = mean_df[mean_df['NumSTAs'].isin(np.arange(0, mean_df['NumSTAs'].max() + 1, 2) + 1)]
filtered_df = pd.concat([mean_df[mean_df['NumSTAs'] == 0], filtered_df])

plt.figure(figsize=(10, 7))

for frequencyBand, group in filtered_df.groupby('FrequencyBand(Hz)'):
    plt.plot(group['NumSTAs'], group['Mean_Goodput(Mbps)'], marker='o', label=f'{frequencyBand} GHz')

plt.xlabel('NumSTAs')
plt.ylabel('Mean Goodput (Mbps)')
plt.title('Mean Goodput vs STAs - 802.11be - Minstrel')

plt.xticks(np.arange(0, filtered_df['NumSTAs'].max() + 1, 5))
plt.yticks(np.arange(0, 2401, 100))

plt.legend(title='Frequency Band')
plt.grid(True)

plt.show()