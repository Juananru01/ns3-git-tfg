#!/bin/bash
#

seed=$1
frequencyBand=$2

if [ "$frequencyBand" == "2.4" ]; then
    maxChannelWidth=40
elif [ "$frequencyBand" == "5" ]; then
    maxChannelWidth=160
elif [ "$frequencyBand" == "6" ]; then
    maxChannelWidth=160
else
    echo "Valor de frequencyBand no válido. Debe ser 2.4, 5 o 6 (GHz)."
    exit 1
fi

# Encabezado del archivo CSV
echo "PacketsInQueue,Time(ms),NumSTAs,FrequencyBand(GHz),ChannelWidth(MHz),Seed,TotalSimulationTime(s)" > dropPackets_vs_tiempo_802.11be_maxWidthChannels_$frequencyBand'GHz'_gi800_seed$seed.csv

numStations=150
simulationTime=1.5

# Ejecutar el comando y almacenar la salida en una variable
output=$(../ns3 run "scratch/dropPackets_vs_tiempo_802.11be --numStations=$numStations --channelWidth=$maxChannelWidth --frequency=$frequencyBand --simulationTime=$simulationTime --guard_interval=800 --seed=$seed");

echo "$output" | while IFS=', ' read -r line; do
    # Extraer el tiempo y los paquetes de la línea
    time=$(echo "$line" | grep -oP 'Time: \K[0-9.]+')
    packets=$(echo "$line" | grep -oP 'Packets in Queue: \K[0-9]+')

    # Escribir los datos en el archivo CSV
    echo "$packets,$time,$numStations,$frequencyBand,$maxChannelWidth,$seed,$simulationTime" >> dropPackets_vs_tiempo_802.11be_maxWidthChannels_${frequencyBand}GHz_gi800_seed${seed}.csv
done