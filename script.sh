# Encabezado del archivo CSV
echo "MCS,CtlrRate,Mean_Throughput(Mbps),Distance(m),SimulationTime(s)" > resultados_802.11ax.csv

# Bucle para ejecutar los comandos para diferentes valores de numStations
for mcs in {0..11}; do
    for distance in {0..100..5}; do
        # Ejecutar el comando y almacenar la salida en una variable
        output=$(./ns3 run "scratch/throughputvalue_vs_distance_802.11ax --mcs=$mcs --distance=$distance");
        
        # Extraer los datos relevantes y escribirlos en el archivo CSV
        mcs=$(echo "$output" | grep -oP 'MCS: HeMcs\K[0-9]+')
        ctrlRate=$(echo "$output" | grep -oP 'CtrlRate: \K[^,]+')
        meanThroughput=$(echo "$output" | grep -oP 'Throughput medio simulación: \K[0-9.]+')
        distance=$(echo "$output" | grep -oP 'Distance: \K[0-9.]+')
        simulationTime=$(echo "$output" | grep -oP 'Simulation Time: \K[0-9]+')
        
        echo "$mcs,$ctrlRate,$meanThroughput,$distance,$simulationTime" >> resultados_802.11ax.csv
    done
done
