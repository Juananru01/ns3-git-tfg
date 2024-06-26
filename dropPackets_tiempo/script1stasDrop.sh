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
echo "QueueDropPackets,PacketsInQueue,PacketsDropPhyRx,PacketsDropPhyRxInterferences,Time(s),NumSTAs,FrequencyBand(GHz),ChannelWidth(MHz),Seed,TotalSimulationTime(s)" > dropPackets_vs_tiempo_802.11ax_maxWidthChannels_$frequencyBand'GHz'_gi800_seed$seed.csv

numStations=40
simulationTime=60

# Ejecutar el comando y almacenar la salida en una variable
output=$(../ns3 run "scratch/dropPackets_vs_tiempo_802.11ax --numStations=$numStations --channelWidth=$maxChannelWidth --frequency=$frequencyBand --simulationTime=$simulationTime --guard_interval=800 --seed=$seed");

# Procesar la salida para extraer los datos por cada segundo
for time in $(seq 0 $simulationTime); do
    line=$(echo "$output" | grep "Time: $time s,")
    if [ -n "$line" ]; then
        dropPhyRx=$(echo "$line" | grep -oP 'Packets dropped in PhyRx: \K[0-9]+')
        dropPhyRxInterferences=$(echo "$line" | grep -oP 'Packets dropped in PhyRx by Interferences: \K[0-9]+')
        packetsInQueue=$(echo "$line" | grep -oP 'Packets in Queue: \K[0-9]+')
        queueDropPackets=$(echo "$line" | grep -oP 'Queue Drop Packets: \K[0-9]+')
        
        # Solo añadir al CSV si se han encontrado todos los datos
        if [[ -n $dropPhyRx && -n $dropPhyRxInterferences && -n $packetsInQueue && -n $queueDropPackets ]]; then
            output_line="$queueDropPackets,$packetsInQueue,$dropPhyRx,$dropPhyRxInterferences,$time,$numStations,$frequencyBand,$maxChannelWidth,$seed,$simulationTime"
            echo "$output_line" >> dropPackets_vs_tiempo_802.11ax_maxWidthChannels_${frequencyBand}GHz_gi800_seed${seed}.csv
        fi
    fi
done




