#!/bin/bash
#

seed=$1

# Encabezado del archivo CSV
echo "Mean_Goodput(Mbps),NumSTAs,FrequencyBand(Hz),ChannelWidth(GHz),Seed,SimulationTime(s)" > goodput_vs_stas_802.11ax_minstrel_maxWidthChannels_gi800_seed$seed.csv

#maxChannelWidth=40

# Recorrer cada valor de channelWidth
#for numStations in {1..60..1}; do
    # Ejecutar el comando y almacenar la salida en una variable
#    output=$(./ns3 run "scratch/goodput_vs_stas_802.11ax_minstrel --numStations=$numStations --channelWidth=$maxChannelWidth --frequency=2.4 --simulationTime=25 --guard_interval=800 --seed=$seed");
    
    # Extraer los datos relevantes y escribirlos en el archivo CSV
#    meanThroughput=$(echo "$output" | grep -oP 'Throughput medio TOTAL de la simulación: \K[0-9.]+')
#    simulationTime=$(echo "$output" | grep -oP 'Simulation Time: \K[0-9]+')
    
#    echo "$meanThroughput,$numStations,2.4,$maxChannelWidth,$seed,$simulationTime" >> goodput_vs_stas_802.11ax_minstrel_maxWidthChannels_gi800_seed$seed.csv
#done

maxChannelWidth2=160

# Definir los valores de frenquency Bands
frequencyBands=(6)

for frequencyBand in "${frequencyBands[@]}"; do
    for numStations in {38..60..1}; do
        # Ejecutar el comando y almacenar la salida en una variable
        output=$(../ns3 run "scratch/goodput_vs_stas_802.11ax_minstrel --numStations=$numStations --channelWidth=$maxChannelWidth2 --frequency=$frequencyBand --simulationTime=25 --guard_interval=800 --seed=$seed");
        
        # Extraer los datos relevantes y escribirlos en el archivo CSV
        meanThroughput=$(echo "$output" | grep -oP 'Throughput medio TOTAL de la simulación: \K[0-9.]+')
        simulationTime=$(echo "$output" | grep -oP 'Simulation Time: \K[0-9]+')
        
        echo "$meanThroughput,$numStations,$frequencyBand,$maxChannelWidth2,$seed,$simulationTime" >> goodput_vs_stas_802.11ax_minstrel_maxWidthChannels_gi800_seed$seed.csv
    done
done

