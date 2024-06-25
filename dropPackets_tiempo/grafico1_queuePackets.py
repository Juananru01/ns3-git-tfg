import pandas as pd
import matplotlib.pyplot as plt
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

# Leer todos los archivos CSV y almacenarlos en una lista de DataFrames
dataframes = [pd.read_csv(file, sep=',') for file in csv_files]

# Inicializar un DataFrame vacío para almacenar los valores medios y las desviaciones estándar
mean_df = pd.DataFrame()

# Asegurarnos de que todos los DataFrames tengan las mismas filas y columnas
for df in dataframes:
    if mean_df.empty:
        mean_df = df.copy()
        mean_df['PacketsInQueue'] = 0
    else:
        if not df['Time(s)'].equals(mean_df['Time(s)']) or not df['FrequencyBand(GHz)'].equals(mean_df['FrequencyBand(GHz)']):
            raise ValueError("Las filas de los archivos CSV no coinciden exactamente.")

# Calcular la media de 'PacketsInQueue' para cada fila
mean_df['PacketsInQueue'] = sum(df['PacketsInQueue'] for df in dataframes) / len(dataframes)

# Calcular la desviación estándar de 'Mean_Goodput(Mbps)' para cada fila
std_dev = np.std([df['PacketsInQueue'] for df in dataframes], axis=0, ddof=1)
mean_df['Std_PacketsInQueue'] = std_dev

# Eliminar la columna 'Seed' si existe
if 'Seed' in mean_df.columns:
    mean_df = mean_df.drop(columns=['Seed'])

mean_df = mean_df[mean_df['Time(s)'] % 2 == 0]

# Calcular el intervalo de confianza del 95%
confidence_interval = 0.95
z_score = stats.norm.ppf(1 - (1 - confidence_interval) / 2)
mean_df['CI_Lower'] = mean_df['PacketsInQueue'] - z_score * mean_df['Std_PacketsInQueue'] / np.sqrt(len(dataframes))
mean_df['CI_Upper'] = mean_df['PacketsInQueue'] + z_score * mean_df['Std_PacketsInQueue'] / np.sqrt(len(dataframes))

plt.figure(figsize=(11, 7))

# Graficar las líneas para cada frecuencia con barras de error para el intervalo de confianza
for frequencyBand, group in mean_df.groupby('FrequencyBand(GHz)'):
    plt.errorbar(group['Time(s)'], group['PacketsInQueue'], 
                 yerr=[group['PacketsInQueue'] - group['CI_Lower'], group['CI_Upper'] - group['PacketsInQueue']], 
                 fmt='o-', capsize=3, elinewidth=1.5, capthick=1.5, label=f'{frequencyBand} GHz')

plt.xlabel('Time(s)', fontsize=18)
plt.ylabel('Mean PacketsInQueue', fontsize=18)
#plt.title('Mean Goodput vs STAs - 802.11ax - Minstrel')

plt.xticks(np.arange(0, mean_df['Time(s)'].max() + 1, 5), fontsize=17)
plt.yticks(np.arange(0, 65, 5), fontsize=17)

plt.legend(title='Frequency Band', title_fontsize=15 ,fontsize=14, loc='lower right')
plt.grid(True)

plt.show()
