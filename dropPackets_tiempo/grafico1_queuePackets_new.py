import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from scipy import stats

csv_files = ['dropPackets_tiempo/802.11ax_queuePackets/queuePackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed1.csv',
             'dropPackets_tiempo/802.11ax_queuePackets/queuePackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed2.csv',
             'dropPackets_tiempo/802.11ax_queuePackets/queuePackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed3.csv',
             'dropPackets_tiempo/802.11ax_queuePackets/queuePackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed4.csv',
             'dropPackets_tiempo/802.11ax_queuePackets/queuePackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed5.csv',
             'dropPackets_tiempo/802.11ax_queuePackets/queuePackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed6.csv',
             'dropPackets_tiempo/802.11ax_queuePackets/queuePackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed7.csv',
             'dropPackets_tiempo/802.11ax_queuePackets/queuePackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed8.csv',
             'dropPackets_tiempo/802.11ax_queuePackets/queuePackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed9.csv',
             'dropPackets_tiempo/802.11ax_queuePackets/queuePackets_vs_tiempo_802.11ax_maxWidthChannels_gi800_seed10.csv']

# Leer todos los archivos CSV y almacenarlos en una lista de DataFrames
dataframes = [pd.read_csv(file, sep=',') for file in csv_files]

filtered_dataframes = [df[(df['Time(ms)'] <= 1.35)] for df in dataframes]

# Inicializar un DataFrame vacío para almacenar los valores medios y las desviaciones estándar
mean_df = pd.DataFrame()

# Asegurarnos de que todos los DataFrames tengan las mismas filas y columnas
for df in filtered_dataframes:
    if mean_df.empty:
        mean_df = df.copy()
        mean_df['PacketsInQueue'] = 0
    else:
        if not df['Time(ms)'].equals(mean_df['Time(ms)']) or not df['FrequencyBand(GHz)'].equals(mean_df['FrequencyBand(GHz)']):
            raise ValueError("Las filas de los archivos CSV no coinciden exactamente.")

# Calcular la media de 'PacketsInQueue' para cada fila
mean_df['PacketsInQueue'] = sum(df['PacketsInQueue'] for df in filtered_dataframes) / len(filtered_dataframes)

# Calcular la desviación estándar de 'Mean_Goodput(Mbps)' para cada fila
std_dev = np.std([df['PacketsInQueue'] for df in filtered_dataframes], axis=0, ddof=1)
mean_df['Std_PacketsInQueue'] = std_dev

# Eliminar la columna 'Seed' si existe
if 'Seed' in mean_df.columns:
    mean_df = mean_df.drop(columns=['Seed'])

# Calcular el intervalo de confianza del 95%
confidence_interval = 0.95
z_score = stats.norm.ppf(1 - (1 - confidence_interval) / 2)
mean_df['CI_Lower'] = mean_df['PacketsInQueue'] - z_score * mean_df['Std_PacketsInQueue'] / np.sqrt(len(filtered_dataframes))
mean_df['CI_Upper'] = mean_df['PacketsInQueue'] + z_score * mean_df['Std_PacketsInQueue'] / np.sqrt(len(filtered_dataframes))
mean_df['CI_Lower'] = mean_df['CI_Lower'].apply(lambda x: max(x, 0))
mean_df['CI_Upper'] = mean_df['CI_Upper'].apply(lambda x: min(x, 100))

plt.figure(figsize=(12, 7))

# Graficar las líneas para cada frecuencia con barras de error para el intervalo de confianza
for frequencyBand, group in mean_df.groupby('FrequencyBand(GHz)'):
    group_filtered = group.iloc[::10]
    plt.errorbar(group_filtered['Time(ms)'], group_filtered['PacketsInQueue'], 
                 yerr=[group_filtered['PacketsInQueue'] - group_filtered['CI_Lower'], group_filtered['CI_Upper'] - group_filtered['PacketsInQueue']], 
                 fmt='o-', capsize=3, elinewidth=1.5, capthick=1.5, label=f'{frequencyBand} GHz')

plt.xlabel('Time(ms)', fontsize=18)
plt.ylabel('Mean PacketsInQueue', fontsize=18)

plt.xticks(fontsize=17)
plt.yticks(np.arange(0, 101, 10), fontsize=17)

plt.legend(title='Frequency Band', title_fontsize=15 ,fontsize=14, loc='lower right')
plt.grid(True)

plt.show()