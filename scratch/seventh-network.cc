#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-mac-header.h"

NS_LOG_COMPONENT_DEFINE("wifi-udp");

/**** TRANSMISIÓN desde STAs a AP ****/

using namespace ns3;

double totalBytesReceived;
double throughputInterval = 1;
double simulationTime = 4;
double totalThroughput;
double averageThroughputTotal = 0;

void CalculateStationThroughput()
{
    Time now = Simulator::Now();
    double time = now.GetSeconds();
    double throughput = 0.0;
    
    if (time > 0) {
        if (totalBytesReceived > 0) {
            throughput = (totalBytesReceived * 8) / time / 1000000;
            totalThroughput += throughput;
        }
    }

    std::cout << "AP - Total MBytes received: " << totalBytesReceived/1000000 << ", Total time: " << time << "s, Throughput: " << throughput << " Mbps" << std::endl;

    Simulator::Schedule(Seconds(throughputInterval), &CalculateStationThroughput);

    if (time == simulationTime) {
        std::cout << "************** Throughput TOTAL de la simulación: " << throughput << " Mbps ****************" << std::endl;
    }
}

void PhyRxOkTrace(std::string context, Ptr<const Packet> packet, double snr, WifiMode mode, WifiPreamble preamble){
    WifiMacHeader macHeader;
    packet->PeekHeader(macHeader);

    if(!macHeader.IsCtl()){
        totalBytesReceived += packet->GetSize();
    }
}

int main(int argc, char* argv[])
{
    uint32_t payload = 2078;
    bool tracing = false;
    uint32_t numStations = 3; // Número de STAs

    CommandLine cmd(__FILE__);
    cmd.AddValue("numStations", "Number of wifi STA devices", numStations);
    cmd.AddValue("payload", "Tamaño de datos útiles en paquetes", payload);
    cmd.AddValue("simulationTime", "Simulation time in seconds", simulationTime);
    cmd.AddValue("tracing", "Enable pcap tracing", tracing);
    cmd.Parse(argc, argv);
 
    WifiHelper wifiHelper;
    wifiHelper.SetStandard(WIFI_STANDARD_80211be);

    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel", "Frequency", DoubleValue(5e9));

    YansWifiPhyHelper wifiPhy;
    wifiPhy.SetChannel(wifiChannel.Create());
    wifiPhy.SetErrorRateModel("ns3::YansErrorRateModel");

    NodeContainer networkNodes;
    networkNodes.Create(numStations + 1); // +1 para el nodo AP
    Ptr<Node> apWifiNode = networkNodes.Get(0);
    std::vector<Ptr<Node>> staWifiNodes;
    for (uint32_t i = 1; i <= numStations; ++i)
    {
        staWifiNodes.push_back(networkNodes.Get(i));
    }

    /* Configure AP */
    Ssid ssid = Ssid("second-network");
    WifiMacHelper wifiMac;
    wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));

    NetDeviceContainer apDevice;
    apDevice = wifiHelper.Install(wifiPhy, wifiMac, apWifiNode);

    /* Configure STAs */
    std::vector<NetDeviceContainer> staDevices;
    for (uint32_t i = 0; i < numStations; ++i)
    {
        wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));
        NetDeviceContainer devices = wifiHelper.Install(wifiPhy, wifiMac, staWifiNodes[i]);
        staDevices.push_back(devices);
    }

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0)); // AP en (0,0,0)
    for (uint32_t i = 0; i < numStations; ++i)
    {
        positionAlloc->Add(Vector(1.0 + (40*i), 0.0 + (30*i), 0.0)); // STAs distribuidas en línea
    }

    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(apWifiNode);
    for (uint32_t i = 0; i < numStations; ++i){
        mobility.Install(staWifiNodes[i]);
    }

    InternetStackHelper stack;
    stack.Install(networkNodes);

    Ipv4AddressHelper address;
    address.SetBase("10.0.2.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterface;
    apInterface = address.Assign(apDevice);
    std::vector<Ipv4InterfaceContainer> staInterfaces;

    Config::Connect("/NodeList/0/DeviceList/*/Phy/State/RxOk", MakeCallback(&PhyRxOkTrace));

    for (uint32_t i = 0; i < numStations; i++)
    {
        Ipv4InterfaceContainer interface = address.Assign(staDevices[i]);
        staInterfaces.push_back(interface);

        UdpClientHelper client(apInterface.GetAddress(0), 9);
        client.SetAttribute("MaxPackets", UintegerValue(0));
        client.SetAttribute("Interval", TimeValue(Seconds(0.0001)));
        client.SetAttribute("PacketSize", UintegerValue(payload));

        ApplicationContainer serverApp = client.Install(staWifiNodes[i]);
        serverApp.Start(Seconds(0.0));

    }

    CalculateStationThroughput();

    if (tracing)
    {
        wifiPhy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
        wifiPhy.EnablePcap("AccessPoint", apDevice);
        for (uint32_t i = 0; i < numStations; ++i)
        {
            wifiPhy.EnablePcap("Station" + std::to_string(i + 1), staDevices[i]);
        }
    }

    /* Start Simulation */
    Simulator::Stop(Seconds(simulationTime+throughputInterval));
    Simulator::Run();


    double averageThroughput = totalThroughput / (simulationTime/throughputInterval);
    averageThroughputTotal = averageThroughputTotal + averageThroughput;
    std::cout << "************** Throughput medio simulación del AP: " << averageThroughput << " Mbps ****************" << std::endl;
    std::cout << "************** Throughput medio TOTAL de la simulación: " << averageThroughputTotal << " Mbps ****************" << std::endl;

    Simulator::Destroy();

    return 0;
}
