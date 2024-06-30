import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from scipy import stats

csv_files = ['goodput_vs_distance/802.11ax/resultados_802.11ax_v2_6GHz_channelWidth160MHz_gi800_seed1.csv',
             'goodput_vs_distance/802.11ax/resultados_802.11ax_v2_6GHz_channelWidth160MHz_gi800_seed2.csv',
             'goodput_vs_distance/802.11ax/resultados_802.11ax_v2_6GHz_channelWidth160MHz_gi800_seed3.csv',
             'goodput_vs_distance/802.11ax/resultados_802.11ax_v2_6GHz_channelWidth160MHz_gi800_seed4.csv',
             'goodput_vs_distance/802.11ax/resultados_802.11ax_v2_6GHz_channelWidth160MHz_gi800_seed5.csv',
             'goodput_vs_distance/802.11ax/resultados_802.11ax_v2_6GHz_channelWidth160MHz_gi800_seed6.csv',
             'goodput_vs_distance/802.11ax/resultados_802.11ax_v2_6GHz_channelWidth160MHz_gi800_seed7.csv',
             'goodput_vs_distance/802.11ax/resultados_802.11ax_v2_6GHz_channelWidth160MHz_gi800_seed8.csv',
             'goodput_vs_distance/802.11ax/resultados_802.11ax_v2_6GHz_channelWidth160MHz_gi800_seed9.csv',
             'goodput_vs_distance/802.11ax/resultados_802.11ax_v2_6GHz_channelWidth160MHz_gi800_seed10.csv']

dataframes = [pd.read_csv(file, sep=',') for file in csv_files]

filtered_dataframes = [df[(df['Distance(m)'] <= 70)] for df in dataframes]

mean_df = pd.DataFrame()

for df in filtered_dataframes:
    if mean_df.empty:
        mean_df = df.copy()
        mean_df['Mean_Goodput(Mbps)'] = 0
    else:
        if not df['Distance(m)'].equals(mean_df['Distance(m)']) or not df['MCS'].equals(mean_df['MCS']):
            raise ValueError("Las filas de los archivos CSV no coinciden exactamente.")

mean_df['Mean_Goodput(Mbps)'] = sum(df['Mean_Goodput(Mbps)'] for df in filtered_dataframes) / len(filtered_dataframes)

std_dev = np.std([df['Mean_Goodput(Mbps)'] for df in filtered_dataframes], axis=0, ddof=1)
mean_df['Std_Goodput(Mbps)'] = std_dev

if 'Seed' in mean_df.columns:
    mean_df = mean_df.drop(columns=['Seed'])

confidence_interval = 0.95
z_score = stats.norm.ppf(1 - (1 - confidence_interval) / 2)
mean_df['CI_Lower'] = mean_df['Mean_Goodput(Mbps)'] - z_score * mean_df['Std_Goodput(Mbps)'] / np.sqrt(len(filtered_dataframes))
mean_df['CI_Upper'] = mean_df['Mean_Goodput(Mbps)'] + z_score * mean_df['Std_Goodput(Mbps)'] / np.sqrt(len(filtered_dataframes))

plt.figure(figsize=(10, 6))

colors = {
    10: 'orange',
    11: 'brown'
}

for mcs, group in mean_df.groupby('MCS'):
    color = colors.get(mcs, None)
    plt.errorbar(group['Distance(m)'], group['Mean_Goodput(Mbps)'], 
                 yerr=[group['Mean_Goodput(Mbps)'] - group['CI_Lower'], group['CI_Upper'] - group['Mean_Goodput(Mbps)']], 
                 fmt='o-', capsize=3, elinewidth=1.5, capthick=1.5, label=f'MCS {mcs}', color=color)

plt.xlabel('Distance (m)', fontsize=18)
plt.ylabel('Mean Goodput (Mbps)', fontsize=18)
#plt.title('Mean Goodput vs Distance - 802.11ax (6GHz)')

plt.ylim(top=800+10)
plt.yticks(np.arange(mean_df['Mean_Goodput(Mbps)'].min(), 801, 50), fontsize=17)
plt.xticks(fontsize=17)

plt.legend(fontsize=16)
plt.grid(True)

plt.subplots_adjust(left=0.1, right=0.95, top=0.95, bottom=0.1)
plt.show()