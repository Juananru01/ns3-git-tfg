# Encabezado del archivo CSV
echo "Mean_Goodput(Mbps),Distance(m),FrequencyBand(GHz),ChannelWidth(MHz),SimulationTime(s)" > goodput_vs_distance_802.11be_minstrel_maxWidthChannels_gi800.csv

maxChannelWidth=40
# Definir los valores de frenquency Bands
frequencyBands=(5 6)

# Recorrer cada valor de channelWidth
for distance in {0..85..5}; do
    # Ejecutar el comando y almacenar la salida en una variable
    output=$(./ns3 run "scratch/goodput_vs_distance_802.11be_minstrel --channelWidth=$maxChannelWidth --distance=$distance --frequency=2.4 --simulationTime=35 --guard_interval=800");
    
    # Extraer los datos relevantes y escribirlos en el archivo CSV
    meanThroughput=$(echo "$output" | grep -oP 'Throughput medio simulación: \K[0-9.]+')
    distance=$(echo "$output" | grep -oP 'Distance: \K[0-9.]+')
    simulationTime=$(echo "$output" | grep -oP 'Simulation Time: \K[0-9]+')
    
    echo "$meanThroughput,$distance,2.4,$maxChannelWidth,$simulationTime" >> goodput_vs_distance_802.11be_minstrel_maxWidthChannels_gi800.csv
done

maxChannelWidth2=160
for frequencyBand in "${frequencyBands[@]}"; do
    for distance in {0..85..5}; do
        # Ejecutar el comando y almacenar la salida en una variable
        output=$(./ns3 run "scratch/goodput_vs_distance_802.11be_minstrel --channelWidth=$maxChannelWidth2 --distance=$distance --frequency=$frequencyBand --simulationTime=35 --guard_interval=800");
        
        # Extraer los datos relevantes y escribirlos en el archivo CSV
        meanThroughput=$(echo "$output" | grep -oP 'Throughput medio simulación: \K[0-9.]+')
        distance=$(echo "$output" | grep -oP 'Distance: \K[0-9.]+')
        simulationTime=$(echo "$output" | grep -oP 'Simulation Time: \K[0-9]+')
        
        echo "$meanThroughput,$distance,$frequencyBand,$maxChannelWidth2,$simulationTime" >> goodput_vs_distance_802.11be_minstrel_maxWidthChannels_gi800.csv
    done
done