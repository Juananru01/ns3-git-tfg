import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

from scipy import stats

csv_files = ['goodput_vs_mcs/goodput_vs_mcs_802.11ax_6GHz_seed1.csv',
             'goodput_vs_mcs/goodput_vs_mcs_802.11ax_6GHz_seed2.csv',
             'goodput_vs_mcs/goodput_vs_mcs_802.11ax_6GHz_seed3.csv',
             'goodput_vs_mcs/goodput_vs_mcs_802.11ax_6GHz_seed4.csv',
             'goodput_vs_mcs/goodput_vs_mcs_802.11ax_6GHz_seed5.csv',
             'goodput_vs_mcs/goodput_vs_mcs_802.11ax_6GHz_seed6.csv',
             'goodput_vs_mcs/goodput_vs_mcs_802.11ax_6GHz_seed7.csv',
             'goodput_vs_mcs/goodput_vs_mcs_802.11ax_6GHz_seed8.csv',
             'goodput_vs_mcs/goodput_vs_mcs_802.11ax_6GHz_seed9.csv',
             'goodput_vs_mcs/goodput_vs_mcs_802.11ax_6GHz_seed10.csv']

# Leer todos los archivos CSV y almacenarlos en una lista de DataFrames
dataframes = [pd.read_csv(file, sep=',') for file in csv_files]

filtered_dataframes = [df[(df['GI(ns)'] == 800)] for df in dataframes]
filtered_dataframes2 = [df[(df['GI(ns)'] == 3200)] for df in dataframes]

# Inicializar un DataFrame vacío para almacenar los valores medios y las desviaciones estándar
mean_df = pd.DataFrame()
mean_df2 = pd.DataFrame()

# Asegurarnos de que todos los DataFrames tengan las mismas filas y columnas
for df in filtered_dataframes:
    if mean_df.empty:
        mean_df = df.copy()
        mean_df['Mean_Goodput(Mbps)'] = 0
    else:
        if not df['MCS'].equals(mean_df['MCS']) or not df['ChannelWidth(Hz)'].equals(mean_df['ChannelWidth(Hz)']):
            raise ValueError("Las filas de los archivos CSV no coinciden exactamente.")

for df in filtered_dataframes2:
    if mean_df2.empty:
        mean_df2 = df.copy()
        mean_df2['Mean_Goodput(Mbps)'] = 0
    else:
        if not df['MCS'].equals(mean_df2['MCS']) or not df['ChannelWidth(Hz)'].equals(mean_df2['ChannelWidth(Hz)']):
            raise ValueError("Las filas de los archivos CSV no coinciden exactamente.")

# Calcular la media de 'Mean_Goodput(Mbps)' para cada fila
mean_df['Mean_Goodput(Mbps)'] = sum(df['Mean_Goodput(Mbps)'] for df in filtered_dataframes) / len(filtered_dataframes)
mean_df2['Mean_Goodput(Mbps)'] = sum(df['Mean_Goodput(Mbps)'] for df in filtered_dataframes2) / len(filtered_dataframes2)

# Calcular la desviación estándar de 'Mean_Goodput(Mbps)' para cada fila
std_dev = np.std([df['Mean_Goodput(Mbps)'] for df in filtered_dataframes], axis=0, ddof=1)
mean_df['Std_Goodput(Mbps)'] = std_dev
std_dev2 = np.std([df['Mean_Goodput(Mbps)'] for df in filtered_dataframes2], axis=0, ddof=1)
mean_df2['Std_Goodput(Mbps)'] = std_dev2


# Eliminar la columna 'Seed' si existe
if 'Seed' in mean_df.columns:
    mean_df = mean_df.drop(columns=['Seed'])

if 'Seed' in mean_df2.columns:
    mean_df2 = mean_df2.drop(columns=['Seed'])

# Calcular el intervalo de confianza del 95%
confidence_interval = 0.95
z_score = stats.norm.ppf(1 - (1 - confidence_interval) / 2)
mean_df['CI_Lower'] = mean_df['Mean_Goodput(Mbps)'] - z_score * mean_df['Std_Goodput(Mbps)'] / np.sqrt(len(filtered_dataframes))
mean_df['CI_Upper'] = mean_df['Mean_Goodput(Mbps)'] + z_score * mean_df['Std_Goodput(Mbps)'] / np.sqrt(len(filtered_dataframes))
mean_df2['CI_Lower'] = mean_df2['Mean_Goodput(Mbps)'] - z_score * mean_df2['Std_Goodput(Mbps)'] / np.sqrt(len(filtered_dataframes2))
mean_df2['CI_Upper'] = mean_df2['Mean_Goodput(Mbps)'] + z_score * mean_df2['Std_Goodput(Mbps)'] / np.sqrt(len(filtered_dataframes2))

plt.figure(figsize=(10, 7))

colors = {
    20: 'green',
    40: 'blue',
    80: 'purple',
    160: 'red'
}

for channelWidth, group in mean_df.groupby('ChannelWidth(Hz)'):
    color = colors.get(channelWidth, None)
    plt.errorbar(group['MCS'], group['Mean_Goodput(Mbps)'], 
                 yerr=[group['Mean_Goodput(Mbps)'] - group['CI_Lower'], group['CI_Upper'] - group['Mean_Goodput(Mbps)']], 
                 fmt='o-', capsize=3, elinewidth=1.5, capthick=1.5, marker='o', label=f'{channelWidth} Hz - 800 ns', color = color)
    
for channelWidth, group in mean_df2.groupby('ChannelWidth(Hz)'):
    color = colors.get(channelWidth, None)
    plt.errorbar(group['MCS'], group['Mean_Goodput(Mbps)'], 
                 yerr=[group['Mean_Goodput(Mbps)'] - group['CI_Lower'], group['CI_Upper'] - group['Mean_Goodput(Mbps)']], 
                 fmt='o-', capsize=3, elinewidth=1.5, capthick=1.5, marker='o', label=f'{channelWidth} Hz - 3200 ns', linestyle=":", color = color)
    
plt.xlabel('MCS', fontsize=22)
plt.ylabel('Mean Goodput (Mbps)', fontsize=22)
#plt.title('Mean Goodput vs MCS - 802.11ax (6GHz)')

plt.ylim(top=800+10)
plt.xticks(np.arange(0, mean_df['MCS'].max() + 1, 1), fontsize=21)
plt.yticks(np.arange(0, 801, 50), fontsize=21)

plt.legend(title='Channel Width - GI', loc='upper left', title_fontsize=18, fontsize=17)
plt.grid(True)

plt.subplots_adjust(left=0.12, right=0.95, top=0.95, bottom=0.12)
plt.show()