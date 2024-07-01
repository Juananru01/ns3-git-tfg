#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/he-phy.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/rng-seed-manager.h"

//
// The simulation assumes a configurable number of stations in an infrastructure network:
//
//  STA     AP
//    *     *
//    |     |
//   n1     n2
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("he-wifi-network");

double totalBytesReceived = 0;
double throughputInterval = 1;        //seconds
int timeInitCountMeanThroughput = 5;
double simulationTime = 25;     //seconds
double totalThroughput = 0;
int numIntervals = 0; 

void CalculateThroughput()
{
    Time now = Simulator::Now();
    double time = now.GetSeconds();
    double throughput = (totalBytesReceived * 8) / time / 1000000;
    if(time == 0) {
        throughput = 0;
    }
    if(time >= timeInitCountMeanThroughput) {
        totalThroughput += throughput;
        numIntervals++;
    }
    //std::cout << "Total MBytes received: " << totalBytesReceived / 1000000 << ", Total time: " << time << "s, Throughput: " << throughput << " Mbps" << std::endl;
    Simulator::Schedule(Seconds(throughputInterval), &CalculateThroughput);
}

void PhyRxOkTrace(std::string context, Ptr<const Packet> packet, double snr, WifiMode mode, WifiPreamble preamble){
    WifiMacHeader macHeader;
    packet->PeekHeader(macHeader);

    if(!macHeader.IsCtl() & !macHeader.IsRetry()){
        totalBytesReceived += packet->GetSize();
    }
}

int
main(int argc, char* argv[])
{
    double distance = 3.0;      // meters
    double frequency = 5;       // whether 2.4, 5 or 6 GHz
    uint32_t payloadSize =
        700;
    int channelWidth = 160; // 20, 40, 80, 160 Mhz
    int gi = 800; // guard interval in ns (800, 1600 o 3200)
    uint32_t seed = 1; // semilla

    CommandLine cmd(__FILE__);
    cmd.AddValue("frequency",
                 "Whether working in the 2.4, 5 or 6 GHz band (other values gets rejected)",
                 frequency);
    cmd.AddValue("distance",
                 "Distance in meters between the station and the access point",
                 distance);
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
    networkNodes.Create(2);
    Ptr<Node> apWifiNode = networkNodes.Get(0);
    Ptr<Node> staWifiNode = networkNodes.Get(1);

    NetDeviceContainer apDevice;
    NetDeviceContainer staDevice;
    WifiMacHelper wifiMac;
    WifiHelper wifi;
    std::string channelStr("{0, " + std::to_string(channelWidth) + ", ");

    RngSeedManager::SetSeed(seed);
    RngSeedManager::SetRun(1);

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
        std::ostringstream ossControlMode;
        channelStr += "BAND_5GHZ, 0}";
    }
    else if (frequency == 2.4)
    {
        wifi.SetStandard(WIFI_STANDARD_80211ax);
        std::ostringstream ossControlMode;
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

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper wifiPhy;
    wifiPhy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
    wifiPhy.SetChannel(channel.Create());

    wifiMac.SetType("ns3::StaWifiMac",
                "Ssid",
                SsidValue(ssid));
    wifiPhy.Set("ChannelSettings", StringValue(channelStr));
    staDevice = wifi.Install(wifiPhy, wifiMac, staWifiNode);

    wifiMac.SetType("ns3::ApWifiMac",
                "EnableBeaconJitter",
                BooleanValue(false),
                "Ssid",
                SsidValue(ssid));
    apDevice = wifi.Install(wifiPhy, wifiMac, apWifiNode);

    apDevice.Get(0)->SetAttribute("Mtu", UintegerValue(2000));
    staDevice.Get(0)->SetAttribute("Mtu", UintegerValue(2000));

    // mobility.
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();

    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(distance, 0.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

    mobility.Install(apWifiNode);
    mobility.Install(staWifiNode);

    /* Internet stack*/
    InternetStackHelper stack;
    stack.Install(apWifiNode);
    stack.Install(staWifiNode);

    Ipv4AddressHelper address;
    address.SetBase("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer staNodeInterface;
    Ipv4InterfaceContainer apNodeInterface;

    staNodeInterface = address.Assign(staDevice);
    apNodeInterface = address.Assign(apDevice);

    Config::Connect("/NodeList/1/DeviceList/*/Phy/State/RxOk", MakeCallback(&PhyRxOkTrace));

    /* Setting applications */
    ApplicationContainer serverApp;
    Ipv4InterfaceContainer serverInterface;

    uint32_t representativeMcs = 11;
    const auto maxLoad = HePhy::GetDataRate(representativeMcs, channelWidth, gi, 1);

    // UDP
    uint16_t port = 9;
    UdpServerHelper server(port);
    serverApp = server.Install(staWifiNode);
    serverApp.Start(Seconds(0.0));
    serverApp.Stop(Seconds(simulationTime + 1));
    const auto packetInterval = payloadSize * 8.0 / maxLoad;

    UdpClientHelper client(staNodeInterface.GetAddress(0), port);
    client.SetAttribute("MaxPackets", UintegerValue(4294967295U));
    client.SetAttribute("Interval", TimeValue(Seconds(packetInterval)));
    client.SetAttribute("PacketSize", UintegerValue(payloadSize));
    ApplicationContainer clientApp = client.Install(apWifiNode);
    clientApp.Start(Seconds(1.0));
    clientApp.Stop(Seconds(simulationTime + throughputInterval));
    CalculateThroughput();

    Simulator::Stop(Seconds(simulationTime + throughputInterval));
    Simulator::Run();

    double averageThroughput = totalThroughput / numIntervals;
    std::cout << "************** Throughput medio simulación: " << averageThroughput << " Mbps, Payload Size: " << payloadSize << ", Distance: "<< distance << " meters, Simulation Time: " << simulationTime-timeInitCountMeanThroughput << " seconds" << " ****************" << std::endl;

    Simulator::Destroy();

    return 0;
}

