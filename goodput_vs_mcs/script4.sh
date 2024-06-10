# Encabezado del archivo CSV
echo "MCS,CtlrRate,Mean_Goodput(Mbps),Distance(m),ChannelWidth(Hz),SimulationTime(s)" > goodput_vs_mcs_802.11ax_2.4GHz_gi3200.csv

# Definir los valores de channelWidth
channelWidths=(20 40)

# Recorrer cada valor de channelWidth
for channelWidth in "${channelWidths[@]}"; do
    for mcs in {0..11}; do
        # Ejecutar el comando y almacenar la salida en una variable
        output=$(./ns3 run "scratch/throughputvalue_vs_distance_802.11ax_v2 --mcs=$mcs --channelWidth=$channelWidth --distance=3 --frequency=5 --simulationTime=35 --guard_interval=3200");
        
        # Extraer los datos relevantes y escribirlos en el archivo CSV
        mcs=$(echo "$output" | grep -oP 'MCS: HeMcs\K[0-9]+')
        ctrlRate=$(echo "$output" | grep -oP 'CtrlRate: \K[^,]+')
        meanThroughput=$(echo "$output" | grep -oP 'Throughput medio simulación: \K[0-9.]+')
        distance=$(echo "$output" | grep -oP 'Distance: \K[0-9.]+')
        simulationTime=$(echo "$output" | grep -oP 'Simulation Time: \K[0-9]+')
        
        echo "$mcs,$ctrlRate,$meanThroughput,$distance,$channelWidth,$simulationTime" >> goodput_vs_mcs_802.11ax_2.4GHz_gi3200.csv
    done
done
