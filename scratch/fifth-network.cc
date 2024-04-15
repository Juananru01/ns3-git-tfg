#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"

NS_LOG_COMPONENT_DEFINE("wifi-udp");

using namespace ns3;

double totalBytesReceived = 0;
double throughputInterval = 0.1;
double simulationTime = 20;
double totalThroughput = 0;

void CalculateThroughput()
{
    Time now = Simulator::Now();
    double time = now.GetSeconds();
    double throughput = (totalBytesReceived * 8) / time / 1000000;
    if(time == 0) {
        throughput = 0;
    }
    totalThroughput += throughput;
    std::cout << "Total MBytes received: " << totalBytesReceived / 1000000 << ", Total time: " << time << "s, Throughput: " << throughput << " Mbps" << std::endl;
    Simulator::Schedule(Seconds(throughputInterval), &CalculateThroughput);
}

void PhyRxOkTrace(std::string context, Ptr<const Packet> packet, double snr, WifiMode mode, WifiPreamble preamble){
    totalBytesReceived += packet->GetSize();
}

int main(int argc, char* argv[])
{
    uint32_t payload = 1472;
    bool tracing = false;

    CommandLine cmd(__FILE__);
    cmd.AddValue("payload", "Tamaño de datos útiles en paquetes", payload);
    cmd.AddValue("simulationTime", "Simulation time in seconds", simulationTime);
    cmd.AddValue("throughputInterval", "Throughput print interval in seconds", throughputInterval);
    cmd.AddValue("tracing", "Enable pcap tracing", tracing);
    cmd.Parse(argc, argv);

    WifiHelper wifiHelper;
    //wifiHelper.SetStandard(WIFI_STANDARD_80211n);
    //wifiHelper.SetStandard(WIFI_STANDARD_80211ac);
    //wifiHelper.SetStandard(WIFI_STANDARD_80211ax);
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

    Config::Connect("/NodeList/*/DeviceList/*/Phy/State/RxOk", MakeCallback(&PhyRxOkTrace));

    /* Install UDP Receiver on the access point */
    PacketSinkHelper sinkHelper("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), 9));
    ApplicationContainer sinkApp = sinkHelper.Install(apWifiNode);

    /* Install UDP Transmitter on the station */
    UdpClientHelper client(apInterface.GetAddress(0), 9);
    client.SetAttribute("MaxPackets", UintegerValue(0));
    client.SetAttribute("Interval", TimeValue(Seconds(0.0001)));
    client.SetAttribute("PacketSize", UintegerValue(payload));
    ApplicationContainer serverApp = client.Install(staWifiNode);

    sinkApp.Start(Seconds(0.0));
    serverApp.Start(Seconds(0.0));
    CalculateThroughput();

    if (tracing)
    {
        wifiPhy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
        wifiPhy.EnablePcap("AccessPoint", apDevice);
        wifiPhy.EnablePcap("Station", staDevices);
    }

    /* Start Simulation */
    Simulator::Stop(Seconds(simulationTime+throughputInterval));

    Simulator::Run();

    double averageThroughput = totalThroughput / (simulationTime/throughputInterval);
    std::cout << "************** Throughput medio simulación: " << averageThroughput << " Mbps ****************" << std::endl;

    Simulator::Destroy();

    return 0;
}
