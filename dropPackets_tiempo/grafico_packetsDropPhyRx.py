import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
from scipy import stats

csv_files = ['dropPackets_tiempo/802.11ax/dropPackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed1.csv',
             'dropPackets_tiempo/802.11ax/dropPackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed2.csv',
             'dropPackets_tiempo/802.11ax/dropPackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed3.csv',
             'dropPackets_tiempo/802.11ax/dropPackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed4.csv',
             'dropPackets_tiempo/802.11ax/dropPackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed5.csv',
             'dropPackets_tiempo/802.11ax/dropPackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed6.csv',
             'dropPackets_tiempo/802.11ax/dropPackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed7.csv',
             'dropPackets_tiempo/802.11ax/dropPackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed8.csv',
             'dropPackets_tiempo/802.11ax/dropPackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed9.csv',
             'dropPackets_tiempo/802.11ax/dropPackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed10.csv']

csv_files2 = ['dropPackets_tiempo/802.11be/dropPackets_vs_tiempo_802.11be_maxWidthChannels_gi800_seed1.csv',
             'dropPackets_tiempo/802.11be/dropPackets_vs_tiempo_802.11be_maxWidthChannels_gi800_seed2.csv',
             'dropPackets_tiempo/802.11be/dropPackets_vs_tiempo_802.11be_maxWidthChannels_gi800_seed3.csv',
             'dropPackets_tiempo/802.11be/dropPackets_vs_tiempo_802.11be_maxWidthChannels_gi800_seed4.csv',
             'dropPackets_tiempo/802.11be/dropPackets_vs_tiempo_802.11be_maxWidthChannels_gi800_seed5.csv',
             'dropPackets_tiempo/802.11be/dropPackets_vs_tiempo_802.11be_maxWidthChannels_gi800_seed6.csv',
             'dropPackets_tiempo/802.11be/dropPackets_vs_tiempo_802.11be_maxWidthChannels_gi800_seed7.csv',
             'dropPackets_tiempo/802.11be/dropPackets_vs_tiempo_802.11be_maxWidthChannels_gi800_seed8.csv',
             'dropPackets_tiempo/802.11be/dropPackets_vs_tiempo_802.11be_maxWidthChannels_gi800_seed9.csv',
             'dropPackets_tiempo/802.11be/dropPackets_vs_tiempo_802.11be_maxWidthChannels_gi800_seed10.csv']

# Leer todos los archivos CSV y almacenarlos en una lista de DataFrames
dataframes = [pd.read_csv(file, sep=',') for file in csv_files]
dataframes2 = [pd.read_csv(file, sep=',') for file in csv_files2]

# Inicializar un DataFrame vacío para almacenar los valores medios y las desviaciones estándar
mean_df = pd.DataFrame()
mean_df2 = pd.DataFrame()

# Asegurarnos de que todos los DataFrames tengan las mismas filas y columnas
for df in dataframes:
    if mean_df.empty:
        mean_df = df.copy()
        mean_df['PacketsDropPhyRx'] = 0
    else:
        if not df['Time(s)'].equals(mean_df['Time(s)']) or not df['FrequencyBand(GHz)'].equals(mean_df['FrequencyBand(GHz)']):
            raise ValueError("Las filas de los archivos CSV no coinciden exactamente.")

for df in dataframes2:
    if mean_df2.empty:
        mean_df2 = df.copy()
        mean_df2['PacketsDropPhyRx'] = 0
    else:
        if not df['Time(s)'].equals(mean_df2['Time(s)']) or not df['FrequencyBand(GHz)'].equals(mean_df2['FrequencyBand(GHz)']):
            raise ValueError("Las filas de los archivos CSV no coinciden exactamente.")

# Calcular la media de 'PacketsDropPhyRx' para cada fila
mean_df['PacketsDropPhyRx'] = sum(df['PacketsDropPhyRx'] for df in dataframes) / len(dataframes)
mean_df2['PacketsDropPhyRx'] = sum(df['PacketsDropPhyRx'] for df in dataframes2) / len(dataframes2)

# Calcular la desviación estándar de 'Mean_Goodput(Mbps)' para cada fila
std_dev = np.std([df['PacketsDropPhyRx'] for df in dataframes], axis=0, ddof=1)
mean_df['Std_PacketsDropPhyRx'] = std_dev

std_dev2 = np.std([df['PacketsDropPhyRx'] for df in dataframes2], axis=0, ddof=1)
mean_df2['Std_PacketsDropPhyRx'] = std_dev2

# Eliminar la columna 'Seed' si existe
if 'Seed' in mean_df.columns:
    mean_df = mean_df.drop(columns=['Seed'])

if 'Seed' in mean_df2.columns:
    mean_df2 = mean_df2.drop(columns=['Seed'])

# Filtrar los valores de 'Time(s)' para mostrar solo cada 2 segundos
mean_df = mean_df[mean_df['Time(s)'] % 2 == 0]
mean_df2 = mean_df2[mean_df2['Time(s)'] % 2 == 0]

# Calcular el intervalo de confianza del 95%
confidence_interval = 0.95
z_score = stats.norm.ppf(1 - (1 - confidence_interval) / 2)
mean_df['CI_Lower'] = mean_df['PacketsDropPhyRx'] - z_score * mean_df['Std_PacketsDropPhyRx'] / np.sqrt(len(dataframes))
mean_df['CI_Upper'] = mean_df['PacketsDropPhyRx'] + z_score * mean_df['Std_PacketsDropPhyRx'] / np.sqrt(len(dataframes))
mean_df2['CI_Lower'] = mean_df2['PacketsDropPhyRx'] - z_score * mean_df2['Std_PacketsDropPhyRx'] / np.sqrt(len(dataframes2))
mean_df2['CI_Upper'] = mean_df2['PacketsDropPhyRx'] + z_score * mean_df2['Std_PacketsDropPhyRx'] / np.sqrt(len(dataframes2))

plt.figure(figsize=(11, 7))

colors = {
    2.4: 'green',
    5: 'blue',
    6: 'red'
}

# Graficar las líneas para cada frecuencia con barras de error para el intervalo de confianza
for frequencyBand, group in mean_df.groupby('FrequencyBand(GHz)'):
    color = colors.get(frequencyBand, None)
    plt.errorbar(group['Time(s)'], group['PacketsDropPhyRx'], 
                 yerr=[group['PacketsDropPhyRx'] - group['CI_Lower'], group['CI_Upper'] - group['PacketsDropPhyRx']], 
                 fmt='o-', capsize=3, elinewidth=1.5, capthick=1.5, label=f'{frequencyBand} GHz - Wi-Fi 6', color = color)
    
for frequencyBand, group in mean_df2.groupby('FrequencyBand(GHz)'):
    color = colors.get(frequencyBand, None)
    plt.errorbar(group['Time(s)'], group['PacketsDropPhyRx'], 
                 yerr=[group['PacketsDropPhyRx'] - group['CI_Lower'], group['CI_Upper'] - group['PacketsDropPhyRx']], 
                 fmt='o-', capsize=6, elinewidth=1.5, capthick=1.5, label=f'{frequencyBand} GHz - Wi-Fi 7', marker='s', linestyle=":", color = color)

plt.xlabel('Time(s)', fontsize=18)
plt.ylabel('Mean PacketsDropPhyRx', fontsize=18)

def scientific_notation(x, pos):
    if x == 0:
        return '0'
    return f'{x/1e6:.0f}·10⁶'

ax = plt.gca()
ax.yaxis.set_major_formatter(ticker.FuncFormatter(scientific_notation))

plt.xticks(np.arange(0, mean_df['Time(s)'].max() + 1, 5), fontsize=17)
plt.yticks(np.arange(0, 52000000, 3000000), fontsize=17)

plt.legend(title='Frequency Band - Wi-Fi', title_fontsize=15 ,fontsize=14)
plt.grid(True)

plt.show()
