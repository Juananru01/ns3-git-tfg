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
echo "Mean_Goodput(Mbps),PayloadSize(bytes),Distance(m),FrequencyBand(GHz),ChannelWidth(Hz),SimulationTime(s),Seed" > goodput_vs_payloadsize_802.11ax_maxChannelWidths_$frequencyBand'GHz'_gi800_seed$seed.csv

for payloadSize in {200..10000..200}; do

    output=$(../ns3 run "scratch/goodput_vs_payloadsize_802.11ax_minstrel --payloadSize=$payloadSize --channelWidth=$maxChannelWidth --distance=3 --frequency=$frequencyBand --simulationTime=25 --guard_interval=800 --seed=$seed");
    
    # Extraer los datos relevantes y escribirlos en el archivo CSV
    payloadSize=$(echo "$output" | grep -oP 'Payload Size: \K[^,]+')
    meanThroughput=$(echo "$output" | grep -oP 'Throughput medio simulación: \K[0-9.]+')
    distance=$(echo "$output" | grep -oP 'Distance: \K[0-9.]+')
    simulationTime=$(echo "$output" | grep -oP 'Simulation Time: \K[0-9]+')
    
    echo "$meanThroughput,$payloadSize,$distance,$frequencyBand,$maxChannelWidth,$simulationTime,$seed" >> goodput_vs_payloadsize_802.11ax_maxChannelWidths_$frequencyBand'GHz'_gi800_seed$seed.csv
done
