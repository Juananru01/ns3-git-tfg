#!/bin/bash
#

seed=$1

# Encabezado del archivo CSV
echo "MCS,CtlrRate,Mean_Throughput(Mbps),Distance(m),SimulationTime(s),Seed" > resultados_802.11be_5GHz_channelWidth160MHz_gi800_seed$seed.csv

# Bucle para ejecutar los comandos para diferentes valores de numStations
for mcs in {0..13}; do
    for distance in {0..100..5}; do
        # Ejecutar el comando y almacenar la salida en una variable
        output=$(../ns3 run "scratch/throughputvalue_vs_distance_802.11be --mcs=$mcs --distance=$distance --frequency=5 --simulationTime=25 --channelWidth=160 --guard_interval=800 --seed=$seed");
        
        # Extraer los datos relevantes y escribirlos en el archivo CSV
        mcs=$(echo "$output" | grep -oP 'MCS: EhtMcs\K[0-9]+')
        ctrlRate=$(echo "$output" | grep -oP 'CtrlRate: \K[^,]+')
        meanThroughput=$(echo "$output" | grep -oP 'Throughput medio simulación: \K[0-9.]+')
        distance=$(echo "$output" | grep -oP 'Distance: \K[0-9.]+')
        simulationTime=$(echo "$output" | grep -oP 'Simulation Time: \K[0-9]+')
        
        echo "$mcs,$ctrlRate,$meanThroughput,$distance,$simulationTime,$seed" >> resultados_802.11be_5GHz_channelWidth160MHz_gi800_seed$seed.csv
    done
done
