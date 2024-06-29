# Encabezado del archivo CSV
echo "Mean_Goodput(Mbps),PayloadSize(bytes),Distance(m),FrequencyBand(GHz),ChannelWidth(Hz),SimulationTime(s)" > goodput_vs_payloadsize_802.11be_maxChannelWidths_gi800.csv

#channelWidth=40

#for payloadSize in {200..10000..200}; do
    # Ejecutar el comando y almacenar la salida en una variable
#    output=$(./ns3 run "scratch/goodput_vs_payloadsize_802.11be_minstrel --payloadSize=$payloadSize --channelWidth=$channelWidth --distance=3 --frequency=2.4 --simulationTime=25 --guard_interval=800");
    
    # Extraer los datos relevantes y escribirlos en el archivo CSV
#    payloadSize=$(echo "$output" | grep -oP 'Payload Size: \K[^,]+')
#    meanThroughput=$(echo "$output" | grep -oP 'Throughput medio simulación: \K[0-9.]+')
#    distance=$(echo "$output" | grep -oP 'Distance: \K[0-9.]+')
#    simulationTime=$(echo "$output" | grep -oP 'Simulation Time: \K[0-9]+')
    
#    echo "$meanThroughput,$payloadSize,$distance,2.4,$channelWidth,$simulationTime" >> goodput_vs_payloadsize_802.11be_maxChannelWidths_gi800.csv
#done

# Definir los valores de channelWidth
frequencyBands=(5 6)
channelWidth2=160

# Recorrer cada valor de channelWidth
for frequencyBand in "${frequencyBands[@]}"; do
    for payloadSize in {200..10000..200}; do
        # Ejecutar el comando y almacenar la salida en una variable
        output=$(./ns3 run "scratch/goodput_vs_payloadsize_802.11be_minstrel --payloadSize=$payloadSize --channelWidth=$channelWidth2 --distance=3 --frequency=$frequencyBand --simulationTime=25 --guard_interval=800");
        
        # Extraer los datos relevantes y escribirlos en el archivo CSV
        payloadSize=$(echo "$output" | grep -oP 'Payload Size: \K[^,]+')
        meanThroughput=$(echo "$output" | grep -oP 'Throughput medio simulación: \K[0-9.]+')
        distance=$(echo "$output" | grep -oP 'Distance: \K[0-9.]+')
        simulationTime=$(echo "$output" | grep -oP 'Simulation Time: \K[0-9]+')
        
        echo "$meanThroughput,$payloadSize,$distance,$frequencyBand,$channelWidth2,$simulationTime" >> goodput_vs_payloadsize_802.11be_maxChannelWidths_gi800.csv
    done
done