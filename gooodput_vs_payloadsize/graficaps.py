import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

csv=pd.read_csv('gooodput_vs_payloadsize/minstrel-and-mtu/goodput_vs_payloadsize_802.11ax_maxChannelWidths_gi800.csv', sep=',')

dataframe=pd.DataFrame(csv[{"Mean_Goodput(Mbps)","PayloadSize(bytes)","FrequencyBand(GHz)"}])

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

from scipy import stats

csv_files = ['gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_2.4GHz_gi800_seed1.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_2.4GHz_gi800_seed2.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_2.4GHz_gi800_seed3.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_2.4GHz_gi800_seed4.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_2.4GHz_gi800_seed5.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_2.4GHz_gi800_seed6.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_2.4GHz_gi800_seed7.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_2.4GHz_gi800_seed8.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_2.4GHz_gi800_seed9.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_2.4GHz_gi800_seed10.csv']

csv_files2 = ['gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_5GHz_gi800_seed1.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_5GHz_gi800_seed2.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_5GHz_gi800_seed3.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_5GHz_gi800_seed4.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_5GHz_gi800_seed5.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_5GHz_gi800_seed6.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_5GHz_gi800_seed7.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_5GHz_gi800_seed8.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_5GHz_gi800_seed9.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_5GHz_gi800_seed10.csv']

csv_files3 = ['gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_6GHz_gi800_seed1.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_6GHz_gi800_seed2.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_6GHz_gi800_seed3.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_6GHz_gi800_seed4.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_6GHz_gi800_seed5.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_6GHz_gi800_seed6.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_6GHz_gi800_seed7.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_6GHz_gi800_seed8.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_6GHz_gi800_seed9.csv',
             'gooodput_vs_payloadsize/goodput_vs_payloadsize_802.11ax_maxChannelWidths_6GHz_gi800_seed10.csv']

# Leer todos los archivos CSV y almacenarlos en una lista de DataFrames
dataframes = [pd.read_csv(file, sep=',') for file in csv_files]
dataframes2 = [pd.read_csv(file, sep=',') for file in csv_files2]
dataframes3 = [pd.read_csv(file, sep=',') for file in csv_files3]

# Inicializar un DataFrame vacío para almacenar los valores medios y las desviaciones estándar
mean_df = pd.DataFrame()
mean_df2 = pd.DataFrame()
mean_df3 = pd.DataFrame()

# Asegurarnos de que todos los DataFrames tengan las mismas filas y columnas
for df in dataframes:
    if mean_df.empty:
        mean_df = df.copy()
        mean_df['Mean_Goodput(Mbps)'] = 0
    else:
        if not df['PayloadSize(bytes)'].equals(mean_df['PayloadSize(bytes)']) or not df['FrequencyBand(GHz)'].equals(mean_df['FrequencyBand(GHz)']):
            raise ValueError("Las filas de los archivos CSV no coinciden exactamente.")

for df in dataframes2:
    if mean_df2.empty:
        mean_df2 = df.copy()
        mean_df2['Mean_Goodput(Mbps)'] = 0
    else:
        if not df['PayloadSize(bytes)'].equals(mean_df2['PayloadSize(bytes)']) or not df['FrequencyBand(GHz)'].equals(mean_df2['FrequencyBand(GHz)']):
            raise ValueError("Las filas de los archivos CSV no coinciden exactamente.")
        
for df in dataframes3:
    if mean_df3.empty:
        mean_df3 = df.copy()
        mean_df3['Mean_Goodput(Mbps)'] = 0
    else:
        if not df['PayloadSize(bytes)'].equals(mean_df3['PayloadSize(bytes)']) or not df['FrequencyBand(GHz)'].equals(mean_df3['FrequencyBand(GHz)']):
            raise ValueError("Las filas de los archivos CSV no coinciden exactamente.")

# Calcular la media de 'Mean_Goodput(Mbps)' para cada fila
mean_df['Mean_Goodput(Mbps)'] = sum(df['Mean_Goodput(Mbps)'] for df in dataframes) / len(dataframes)
mean_df2['Mean_Goodput(Mbps)'] = sum(df['Mean_Goodput(Mbps)'] for df in dataframes2) / len(dataframes2)
mean_df3['Mean_Goodput(Mbps)'] = sum(df['Mean_Goodput(Mbps)'] for df in dataframes3) / len(dataframes3)

# Calcular la desviación estándar de 'Mean_Goodput(Mbps)' para cada fila
std_dev = np.std([df['Mean_Goodput(Mbps)'] for df in dataframes], axis=0, ddof=1)
mean_df['Std_Goodput(Mbps)'] = std_dev
std_dev2 = np.std([df['Mean_Goodput(Mbps)'] for df in dataframes2], axis=0, ddof=1)
mean_df2['Std_Goodput(Mbps)'] = std_dev2
std_dev3 = np.std([df['Mean_Goodput(Mbps)'] for df in dataframes3], axis=0, ddof=1)
mean_df3['Std_Goodput(Mbps)'] = std_dev3

# Eliminar la columna 'Seed' si existe
if 'Seed' in mean_df.columns:
    mean_df = mean_df.drop(columns=['Seed'])

if 'Seed' in mean_df2.columns:
    mean_df2 = mean_df2.drop(columns=['Seed'])

if 'Seed' in mean_df3.columns:
    mean_df3 = mean_df3.drop(columns=['Seed'])

# Calcular el intervalo de confianza del 95%
confidence_interval = 0.95
z_score = stats.norm.ppf(1 - (1 - confidence_interval) / 2)
mean_df['CI_Lower'] = mean_df['Mean_Goodput(Mbps)'] - z_score * mean_df['Std_Goodput(Mbps)'] / np.sqrt(len(dataframes))
mean_df['CI_Upper'] = mean_df['Mean_Goodput(Mbps)'] + z_score * mean_df['Std_Goodput(Mbps)'] / np.sqrt(len(dataframes))
mean_df2['CI_Lower'] = mean_df2['Mean_Goodput(Mbps)'] - z_score * mean_df2['Std_Goodput(Mbps)'] / np.sqrt(len(dataframes2))
mean_df2['CI_Upper'] = mean_df2['Mean_Goodput(Mbps)'] + z_score * mean_df2['Std_Goodput(Mbps)'] / np.sqrt(len(dataframes2))
mean_df3['CI_Lower'] = mean_df3['Mean_Goodput(Mbps)'] - z_score * mean_df3['Std_Goodput(Mbps)'] / np.sqrt(len(dataframes3))
mean_df3['CI_Upper'] = mean_df3['Mean_Goodput(Mbps)'] + z_score * mean_df3['Std_Goodput(Mbps)'] / np.sqrt(len(dataframes3))

plt.figure(figsize=(12, 7))

colors = {
    2.4: 'green',
    5: 'blue',
    6: 'red'
}

linestyles = {
    2.4: 'solid',
    5: 'solid',
    6: '--'
}

markers = {
    2.4: 'o',
    5: 'o',
    6: 'o'
}

markersizes = {
    2.4: 6,
    5: 7,
    6: 3
}

dashes = {
    2.4: [20,0],
    5: [0,1,0],
    6: [3,0,3]
}

for frequencyBand, group in mean_df.groupby('FrequencyBand(GHz)'):
    color = colors.get(frequencyBand, None)
    linestyle = linestyles.get(frequencyBand, None)
    marker = markers.get(frequencyBand, None)
    markersize = markersizes.get(frequencyBand, None)
    dashe = dashes.get(frequencyBand, None)
    plt.errorbar(group['PayloadSize(bytes)'], group['Mean_Goodput(Mbps)'], 
                 yerr=[group['Mean_Goodput(Mbps)'] - group['CI_Lower'], group['CI_Upper'] - group['Mean_Goodput(Mbps)']], 
                 fmt='o-', capsize=3, elinewidth=1.5, capthick=1.5, marker=marker, markersize=markersize, label=f'{frequencyBand} GHz', color=color, linestyle=linestyle, dashes=(dashe))
    
for frequencyBand, group in mean_df2.groupby('FrequencyBand(GHz)'):
    color = colors.get(frequencyBand, None)
    linestyle = linestyles.get(frequencyBand, None)
    marker = markers.get(frequencyBand, None)
    markersize = markersizes.get(frequencyBand, None)
    dashe = dashes.get(frequencyBand, None)
    plt.errorbar(group['PayloadSize(bytes)'], group['Mean_Goodput(Mbps)'], 
                 yerr=[group['Mean_Goodput(Mbps)'] - group['CI_Lower'], group['CI_Upper'] - group['Mean_Goodput(Mbps)']], 
                 fmt='o-', capsize=3, elinewidth=1.5, capthick=1.5, marker=marker, markersize=markersize, label=f'{frequencyBand} GHz', color=color, linestyle=linestyle, dashes=(dashe))

for frequencyBand, group in mean_df3.groupby('FrequencyBand(GHz)'):
    color = colors.get(frequencyBand, None)
    linestyle = linestyles.get(frequencyBand, None)
    marker = markers.get(frequencyBand, None)
    markersize = markersizes.get(frequencyBand, None)
    dashe = dashes.get(frequencyBand, None)
    plt.errorbar(group['PayloadSize(bytes)'], group['Mean_Goodput(Mbps)'], 
                 yerr=[group['Mean_Goodput(Mbps)'] - group['CI_Lower'], group['CI_Upper'] - group['Mean_Goodput(Mbps)']], 
                 fmt='o-', capsize=3, elinewidth=1.5, capthick=1.5, marker=marker, markersize=markersize, label=f'{frequencyBand} GHz', color=color, linestyle=linestyle, dashes=(dashe))
       
plt.xlabel('PayloadSize(bytes)', fontsize=18)
plt.ylabel('Mean Goodput (Mbps)', fontsize=18)
#plt.title('Mean Goodput vs Payload Size - 802.11ax - Minstrel')

plt.ylim(top=600+5)
plt.xticks(np.arange(0, mean_df['PayloadSize(bytes)'].max() + 5, 1000), fontsize=17)
plt.yticks(np.arange(0, 601, 25), fontsize=17)

plt.legend(title='Frequency Band', title_fontsize=16, fontsize=15)
plt.grid(True)

plt.subplots_adjust(left=0.1, right=0.95, top=0.95, bottom=0.1)
plt.show()