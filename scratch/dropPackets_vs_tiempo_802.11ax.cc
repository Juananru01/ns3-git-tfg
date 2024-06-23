#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-mac-header.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/traffic-control-module.h"

NS_LOG_COMPONENT_DEFINE("wifi-udp");

/**** TRANSMISIÓN desde AP a STAs ****/

using namespace ns3;

double interval = 1;
double simulationTime = 3;

uint32_t packetsDroppedPhyRx = 0;
uint32_t packetsDroppedPhyRxInterferences = 0;


void PhyRxDrop(std::string context, Ptr<const Packet> packet, WifiPhyRxfailureReason reason) {
    switch (reason) {
        case WifiPhyRxfailureReason::PREAMBLE_DETECT_FAILURE:
        case WifiPhyRxfailureReason::L_SIG_FAILURE:
        case WifiPhyRxfailureReason::HT_SIG_FAILURE:
        case WifiPhyRxfailureReason::SIG_A_FAILURE:
        case WifiPhyRxfailureReason::SIG_B_FAILURE:
        case WifiPhyRxfailureReason::U_SIG_FAILURE:
        case WifiPhyRxfailureReason::EHT_SIG_FAILURE:
            packetsDroppedPhyRxInterferences++;
            break;
        default:
            break;
    }
    packetsDroppedPhyRx++;
}

uint32_t oldPacketsInQueue = 0;
uint32_t newPacketsInQueue = 0;

void PacketsInQueueTrace(uint32_t oldValue, uint32_t newValue)
{
    oldPacketsInQueue = oldValue;
    newPacketsInQueue = newValue;
}

uint32_t dropsObserved = 0; 
void TraceQueueDrop(Ptr<const QueueDiscItem> item) {
    dropsObserved++;
}

void PrintData() {
    std::cout << "Time: " << Simulator::Now().GetSeconds() 
              << " s, Packets dropped in PhyRx: " << packetsDroppedPhyRx
              << ", Packets dropped in PhyRx by Interferences: " << packetsDroppedPhyRxInterferences
              << ", Packets in Queue: " << newPacketsInQueue
              << ", Queue Drop Packets: " << dropsObserved << std::endl;
    Simulator::Schedule(Seconds(interval), &PrintData);
}

int main(int argc, char* argv[])
{
    uint32_t numStations = 4; // Número de STAs

    double maxX = 50.0; // límite eje X
    double maxY = 50.0; // límite eje Y
    double maxZ = 25.0;  // límite eje Z (0 en un plano 2D)
    uint32_t seed = 1; // semilla

    double frequency = 5;       // whether 2.4, 5 or 6 GHz
    uint32_t payloadSize = 1024;
    int channelWidth = 160; // 20, 40, 80, 160 Mhz
    int gi = 800; // guard interval in ns (800, 1600 o 3200)

    CommandLine cmd(__FILE__);
    cmd.AddValue("numStations", "Number of wifi STA devices", numStations);
    cmd.AddValue("frequency",
                "Whether working in the 2.4, 5 or 6 GHz band (other values gets rejected)",
                frequency);
    cmd.AddValue("guard_interval",
                 "Guard interval (800, 1600, 3200) in ns",
                 gi);
    cmd.AddValue("channelWidth",
                 "Channel width in MHz (20, 40, 80, 160)",
                 channelWidth);
    cmd.AddValue("simulationTime", "Simulation time in seconds", simulationTime);
    cmd.AddValue("payloadSize", "The application payload size in bytes", payloadSize);
    cmd.AddValue("seed", "Seed value for random number generator", seed);

    cmd.Parse(argc, argv);

    NodeContainer networkNodes;
    networkNodes.Create(numStations + 1); // +1 para el nodo AP
    Ptr<Node> apWifiNode = networkNodes.Get(0);
    std::vector<Ptr<Node>> staWifiNodes;
    for (uint32_t i = 1; i <= numStations; ++i)
    {
        staWifiNodes.push_back(networkNodes.Get(i));
    }
 
    WifiHelper wifi;
    WifiMacHelper wifiMac;
    std::string channelStr("{0, " + std::to_string(channelWidth) + ", ");

    if (frequency == 6)
    {
        wifi.SetStandard(WIFI_STANDARD_80211ax);
        channelStr += "BAND_6GHZ, 0}";
        Config::SetDefault("ns3::LogDistancePropagationLossModel::ReferenceLoss",
                            DoubleValue(48));
    }
    else if (frequency == 5)
    {
        wifi.SetStandard(WIFI_STANDARD_80211ax);
        channelStr += "BAND_5GHZ, 0}";
    }
    else if (frequency == 2.4)
    {
        wifi.SetStandard(WIFI_STANDARD_80211ax);
        channelStr += "BAND_2_4GHZ, 0}";
        Config::SetDefault("ns3::LogDistancePropagationLossModel::ReferenceLoss",
                            DoubleValue(40));
    }
    else
    {
        std::cout << "Wrong frequency value!" << std::endl;
        return 0;
    }

    wifi.SetRemoteStationManager("ns3::MinstrelHtWifiManager");

    // Set guard interval
    wifi.ConfigHeOptions("GuardInterval", TimeValue(NanoSeconds(gi)));

    Ssid ssid = Ssid("ns3-80211ax");

    NetDeviceContainer apDevice;
    std::vector<NetDeviceContainer> staDevices;

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper wifiPhy;
    wifiPhy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
    wifiPhy.SetChannel(channel.Create());

    wifiMac.SetType("ns3::StaWifiMac",
        "Ssid",
        SsidValue(ssid));
    wifiPhy.Set("ChannelSettings", StringValue(channelStr));
    for (uint32_t i = 0; i < numStations; ++i)
    {
        NetDeviceContainer devices = wifi.Install(wifiPhy, wifiMac, staWifiNodes[i]);
        staDevices.push_back(devices);
    }

    wifiMac.SetType("ns3::ApWifiMac",
                "EnableBeaconJitter",
                BooleanValue(false),
                "Ssid",
                SsidValue(ssid));
    apDevice = wifi.Install(wifiPhy, wifiMac, apWifiNode);

    RngSeedManager::SetSeed(seed);
    RngSeedManager::SetRun(1);

    MobilityHelper mobilityAp;
    Ptr<ListPositionAllocator> positionAllocAp = CreateObject<ListPositionAllocator>();
    positionAllocAp->Add(Vector(0.0, 0.0, 0.0)); // AP en (0,0,0)
    mobilityAp.SetPositionAllocator(positionAllocAp);
    mobilityAp.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilityAp.Install(apWifiNode);

    MobilityHelper mobilityStas;
    Ptr<ListPositionAllocator> positionAllocStas = CreateObject<ListPositionAllocator>();

    Ptr<UniformRandomVariable> randX = CreateObject<UniformRandomVariable>();
    randX->SetAttribute("Min", DoubleValue(0.0));
    randX->SetAttribute("Max", DoubleValue(maxX));

    Ptr<UniformRandomVariable> randY = CreateObject<UniformRandomVariable>();
    randY->SetAttribute("Min", DoubleValue(0.0));
    randY->SetAttribute("Max", DoubleValue(maxY));

    Ptr<UniformRandomVariable> randZ = CreateObject<UniformRandomVariable>();
    randZ->SetAttribute("Min", DoubleValue(0.0));
    randZ->SetAttribute("Max", DoubleValue(maxZ));

    for (uint32_t i = 0; i < numStations; ++i){
        double posX = randX->GetValue();
        double posY = randY->GetValue();
        double posZ = randZ->GetValue();
        positionAllocStas->Add(Vector(posX, posY, posZ));
        //std::cout << "************** Posición de STA" << i+1 << " :" << Vector(posX, posY, posZ) << " Mbps ****************" << std::endl;
    }

    mobilityStas.SetPositionAllocator(positionAllocStas);
    mobilityStas.SetMobilityModel("ns3::ConstantPositionMobilityModel");

    for (uint32_t i = 0; i < numStations; ++i){
        mobilityStas.Install(staWifiNodes[i]);
    }

    InternetStackHelper stack;
    stack.Install(apWifiNode);
    for (uint32_t i = 0; i < numStations; ++i){
        stack.Install(staWifiNodes[i]);
    }

    Ipv4AddressHelper address;
    address.SetBase("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterface;
    apInterface = address.Assign(apDevice);
    std::vector<Ipv4InterfaceContainer> staInterfaces;

    for (uint32_t i = 1; i <= numStations; i++){
        Config::Connect("/NodeList/"+std::to_string(i)+"/DeviceList/*/Phy/PhyRxDrop", MakeCallback(&PhyRxDrop));
    }

    TrafficControlHelper tch;
    tch.SetRootQueueDisc("ns3::RedQueueDisc", "MaxSize", StringValue("100p"));
    tch.Uninstall(apDevice);
    QueueDiscContainer qdiscs = tch.Install(apDevice);

    Ptr<QueueDisc> q = qdiscs.Get(0);
    q->TraceConnectWithoutContext("PacketsInQueue", MakeCallback(&PacketsInQueueTrace));
    q->TraceConnectWithoutContext("Drop", MakeBoundCallback(&TraceQueueDrop));

    ApplicationContainer serverApp;
    
    uint32_t representativeMcs = 11;
    const auto maxLoad = HePhy::GetDataRate(representativeMcs, channelWidth, gi, 1);

    uint16_t port = 9;
    UdpServerHelper server(port);
    for (uint32_t i = 0; i < numStations; i++) {
        serverApp = server.Install(staWifiNodes[i]);
    }
    serverApp.Start(Seconds(0.0));
    serverApp.Stop(Seconds(simulationTime));
    const auto packetInterval = payloadSize * 8.0 / maxLoad;

    for (uint32_t i = 0; i < numStations; i++)
    {
        Ipv4InterfaceContainer interface = address.Assign(staDevices[i]);
        staInterfaces.push_back(interface);

        UdpClientHelper client(staInterfaces[i].GetAddress(0), port);
        client.SetAttribute("MaxPackets", UintegerValue(4294967295U));
        client.SetAttribute("Interval", TimeValue(Seconds(packetInterval)));
        client.SetAttribute("PacketSize", UintegerValue(payloadSize));
        ApplicationContainer clientApp = client.Install(apWifiNode);
        clientApp.Start(Seconds(1.0));
        clientApp.Stop(Seconds(simulationTime));
    }

    PrintData();

    /* Start Simulation */
    Simulator::Stop(Seconds(simulationTime));
    Simulator::Run();

    PrintData();

    Simulator::Destroy();

    return 0;
}
