import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from scipy import stats

csv_files = ['goodput_vs_distance_minstrel/802.11be/goodput_vs_distance_802.11be_minstrel_widthChannel40MHz_gi800_seed1.csv',
             'goodput_vs_distance_minstrel/802.11be/goodput_vs_distance_802.11be_minstrel_widthChannel40MHz_gi800_seed2.csv',
             'goodput_vs_distance_minstrel/802.11be/goodput_vs_distance_802.11be_minstrel_widthChannel40MHz_gi800_seed3.csv',
             'goodput_vs_distance_minstrel/802.11be/goodput_vs_distance_802.11be_minstrel_widthChannel40MHz_gi800_seed4.csv',
             'goodput_vs_distance_minstrel/802.11be/goodput_vs_distance_802.11be_minstrel_widthChannel40MHz_gi800_seed5.csv',
             'goodput_vs_distance_minstrel/802.11be/goodput_vs_distance_802.11be_minstrel_widthChannel40MHz_gi800_seed6.csv',
             'goodput_vs_distance_minstrel/802.11be/goodput_vs_distance_802.11be_minstrel_widthChannel40MHz_gi800_seed7.csv',
             'goodput_vs_distance_minstrel/802.11be/goodput_vs_distance_802.11be_minstrel_widthChannel40MHz_gi800_seed8.csv',
             'goodput_vs_distance_minstrel/802.11be/goodput_vs_distance_802.11be_minstrel_widthChannel40MHz_gi800_seed9.csv',
             'goodput_vs_distance_minstrel/802.11be/goodput_vs_distance_802.11be_minstrel_widthChannel40MHz_gi800_seed10.csv']

dataframes = [pd.read_csv(file, sep=',') for file in csv_files]

mean_df = pd.DataFrame()

for df in dataframes:
    if mean_df.empty:
        mean_df = df.copy()
        mean_df['Mean_Goodput(Mbps)'] = 0
    else:
        if not df['Distance(m)'].equals(mean_df['Distance(m)']) or not df['FrequencyBand(GHz)'].equals(mean_df['FrequencyBand(GHz)']):
            raise ValueError("Las filas de los archivos CSV no coinciden exactamente.")

mean_df['Mean_Goodput(Mbps)'] = sum(df['Mean_Goodput(Mbps)'] for df in dataframes) / len(dataframes)

std_dev = np.std([df['Mean_Goodput(Mbps)'] for df in dataframes], axis=0, ddof=1)
mean_df['Std_Goodput(Mbps)'] = std_dev

if 'Seed' in mean_df.columns:
    mean_df = mean_df.drop(columns=['Seed'])

confidence_interval = 0.95
z_score = stats.norm.ppf(1 - (1 - confidence_interval) / 2)
mean_df['CI_Lower'] = mean_df['Mean_Goodput(Mbps)'] - z_score * mean_df['Std_Goodput(Mbps)'] / np.sqrt(len(dataframes))
mean_df['CI_Upper'] = mean_df['Mean_Goodput(Mbps)'] + z_score * mean_df['Std_Goodput(Mbps)'] / np.sqrt(len(dataframes))

plt.figure(figsize=(8, 5))

for frequencyBand, group in mean_df.groupby('FrequencyBand(GHz)'):
    plt.errorbar(group['Distance(m)'], group['Mean_Goodput(Mbps)'], 
                 yerr=[group['Mean_Goodput(Mbps)'] - group['CI_Lower'], group['CI_Upper'] - group['Mean_Goodput(Mbps)']], 
                 fmt='o-', capsize=3, elinewidth=1.5, capthick=1.5, label=f'{frequencyBand} GHz')

plt.xlabel('Distance(m)', fontsize=22)
plt.ylabel('Mean Goodput (Mbps)', fontsize=22)

plt.ylim(top=200+5)
plt.xticks(np.arange(0, mean_df['Distance(m)'].max() + 5, 10), fontsize=21)
plt.yticks(np.arange(0, 201, 20), fontsize=21)

plt.legend(title='Frequency Band', title_fontsize=19, fontsize=18)
plt.grid(True)

plt.subplots_adjust(left=0.15, right=0.90, top=0.90, bottom=0.15)
plt.show()