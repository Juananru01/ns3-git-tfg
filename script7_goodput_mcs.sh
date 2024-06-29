#!/bin/bash
#

seed=$1

# Encabezado del archivo CSV
echo "MCS,CtlrRate,Mean_Goodput(Mbps),Distance(m),ChannelWidth(Hz),GI(ns),SimulationTime(s),Seed" > goodput_vs_mcs_802.11be_2.4GHz_seed$seed.csv

# Definir los valores de channelWidth
channelWidths=(20 40 80 160)
gis=(800 3200)

# Recorrer cada valor de channelWidth
for gi in "${gis[@]}"; do
    for channelWidth in "${channelWidths[@]}"; do
        for mcs in {0..13}; do
            # Ejecutar el comando y almacenar la salida en una variable
            output=$(../ns3 run "scratch/throughputvalue_vs_distance_802.11be --mcs=$mcs --channelWidth=$channelWidth --distance=3 --frequency=2.4 --simulationTime=35 --guard_interval=$gi --seed=$seed");
            
            # Extraer los datos relevantes y escribirlos en el archivo CSV
            mcs=$(echo "$output" | grep -oP 'MCS: EhtMcs\K[0-9]+')
            ctrlRate=$(echo "$output" | grep -oP 'CtrlRate: \K[^,]+')
            meanThroughput=$(echo "$output" | grep -oP 'Throughput medio simulación: \K[0-9.]+')
            distance=$(echo "$output" | grep -oP 'Distance: \K[0-9.]+')
            simulationTime=$(echo "$output" | grep -oP 'Simulation Time: \K[0-9]+')
            
            echo "$mcs,$ctrlRate,$meanThroughput,$distance,$channelWidth,$gi,$simulationTime,$seed" >> goodput_vs_mcs_802.11be_2.4GHz_seed$seed.csv
        done
    done
done