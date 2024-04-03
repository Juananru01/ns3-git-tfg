#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"

NS_LOG_COMPONENT_DEFINE("wifi-udp");

using namespace ns3;

Ptr<PacketSink> sink;

void
CalculateTotalBytes()
{
    Time now = Simulator::Now();
    uint64_t totalBytes = sink->GetTotalRx();
    std::cout << now.GetSeconds() << "s: \tTotal bytes received: " << totalBytes << std::endl;
    Simulator::Schedule(MilliSeconds(1000), &CalculateTotalBytes);
}

void CalculateThroughput()
{
    Time now = Simulator::Now();
    uint64_t totalBytes = sink->GetTotalRx();
    double totalTime = now.GetSeconds();
    double throughput = (totalBytes * 8) / totalTime / 1000000; // Convertir bytes a bits y tiempo a segundos, y luego a Mbps
    std::cout << "Total bytes received: " << totalBytes << ", Total time: " << totalTime << "s, Throughput: " << throughput << " Mbps" << std::endl;
}

int
main(int argc, char* argv[])
{
    uint32_t payload = 1472;
    double simulationTime = 10;
    bool tracing = false;

    CommandLine cmd(__FILE__);
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
    networkNodes.Create(2);
    Ptr<Node> apWifiNode = networkNodes.Get(0);
    Ptr<Node> staWifiNode = networkNodes.Get(1);

    /* Configure AP */
    Ssid ssid = Ssid("second-network");
    WifiMacHelper wifiMac;
    wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));

    NetDeviceContainer apDevice;
    apDevice = wifiHelper.Install(wifiPhy, wifiMac, apWifiNode);

    /* Configure STA */
    wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));

    NetDeviceContainer staDevices;
    staDevices = wifiHelper.Install(wifiPhy, wifiMac, staWifiNode);

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(15.0, 0.0, 0.0));

    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(apWifiNode);
    mobility.Install(staWifiNode);

    InternetStackHelper stack;
    stack.Install(networkNodes);

    Ipv4AddressHelper address;
    address.SetBase("10.0.2.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterface;
    apInterface = address.Assign(apDevice);
    Ipv4InterfaceContainer staInterface;
    staInterface = address.Assign(staDevices);

    /* Install UDP Receiver on the access point */
    PacketSinkHelper sinkHelper("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), 9));
    ApplicationContainer sinkApp = sinkHelper.Install(apWifiNode);
    sink = StaticCast<PacketSink>(sinkApp.Get(0));

    /* Install UDP Transmitter on the station */
    UdpClientHelper client(apInterface.GetAddress(0), 9);
    client.SetAttribute("MaxPackets", UintegerValue(0));
    client.SetAttribute("Interval", TimeValue(Seconds(0.0001)));
    client.SetAttribute("PacketSize", UintegerValue(payload));
    ApplicationContainer serverApp = client.Install(staWifiNode);

    sinkApp.Start(Seconds(0.0));
    serverApp.Start(Seconds(0.0));

    if (tracing)
    {
        wifiPhy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
        wifiPhy.EnablePcap("AccessPoint", apDevice);
        wifiPhy.EnablePcap("Station", staDevices);
    }

    /* Start Simulation */
    Simulator::Stop(Seconds(simulationTime));
    Simulator::Run();

    CalculateThroughput();

    Simulator::Destroy();

    return 0;
}

// ./ns3 run scratch/third-network