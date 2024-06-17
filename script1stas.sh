#!/bin/bash
#

seed=$1
frequencyBand = $2

if [ "$freqBand" == "2.4" ]; then
    maxChannelWidth=40
elif [ "$freqBand" == "5" ]; then
    maxChannelWidth=160
elif [ "$freqBand" == "6" ]; then
    maxChannelWidth=160
else
    echo "Valor de freqBand no válido. Debe ser 2.4, 5 o 6."
    exit 1
fi

# Encabezado del archivo CSV
echo "Mean_Goodput(Mbps),NumSTAs,FrequencyBand(GHz),ChannelWidth(MHz),Seed,SimulationTime(s)" > goodput_vs_stas_802.11ax_minstrel_maxWidthChannels_$freqBandGHz_gi800_seed$seed.csv

maxChannelWidth=40

numStations=1
output=$(./ns3 run "scratch/goodput_vs_stas_802.11ax_minstrel --numStations=$numStations --channelWidth=$maxChannelWidth --frequency=$frequencyBand --simulationTime=25 --guard_interval=800 --seed=$seed")

# Extraer los datos relevantes y escribirlos en el archivo CSV
meanThroughput=$(echo "$output" | grep -oP 'Throughput medio TOTAL de la simulación: \K[0-9.]+')
simulationTime=$(echo "$output" | grep -oP 'Simulation Time: \K[0-9]+')

echo "$meanThroughput,$numStations,$frequencyBand,$maxChannelWidth2,$seed,$simulationTime" >> goodput_vs_stas_802.11ax_minstrel_maxWidthChannels_$freqBandGHz_gi800_seed$seed.csv

# Recorrer cada valor de channelWidth
for numStations in {5..150..5}; do
    # Ejecutar el comando y almacenar la salida en una variable
    output=$(./ns3 run "scratch/goodput_vs_stas_802.11ax_minstrel --numStations=$numStations --channelWidth=$maxChannelWidth --frequency=$frequencyBand --simulationTime=25 --guard_interval=800 --seed=$seed");
    
    # Extraer los datos relevantes y escribirlos en el archivo CSV
    meanThroughput=$(echo "$output" | grep -oP 'Throughput medio TOTAL de la simulación: \K[0-9.]+')
    simulationTime=$(echo "$output" | grep -oP 'Simulation Time: \K[0-9]+')
    
    echo "$meanThroughput,$numStations,2.4,$maxChannelWidth,$seed,$simulationTime" >> goodput_vs_stas_802.11ax_minstrel_maxWidthChannels_$freqBandGHz_gi800_seed$seed.csv
done






