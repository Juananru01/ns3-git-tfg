/*
 * First Network TFG - AP and STA - 802.11
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"

// Default Network Topology
//
//   Wifi 10.1.3.0
//        
//    AP (n0)
//                 
//  *    *    *
//  |    |    |
// n3   n2   n1

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("MyFirstWirelessNetwork");

int
main(int argc, char* argv[])
{
    bool verbose = true;
    uint32_t nPkts = 1;
    uint32_t nSTAs = 2;
    uint32_t packetSize = 1024;
    uint32_t simulationTime = 20;
    bool tracing = false;

    CommandLine cmd(__FILE__);
    cmd.AddValue("nPkts", "Number of Packets", nPkts);
    cmd.AddValue("nSTAs", "Number of wifi STA devices", nSTAs);
    cmd.AddValue("packetSize", "Number of wifi STA devices", packetSize);
    cmd.AddValue("simulationTime", "Simulation time in seconds", simulationTime);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);
    cmd.AddValue("tracing", "Enable pcap tracing", tracing);

    cmd.Parse(argc, argv);

    if (nSTAs > 250)
    {
        std::cout << "Too many WiFi stations, no more than 250 each"
                  << std::endl;
        return 1;
    }

    if (verbose)
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(nSTAs);
    NodeContainer wifiApNode;
    wifiApNode.Create(1); // Creación de 1 AP.

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiMacHelper mac;
    Ssid ssid = Ssid("FirstWifi");

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211be);

    NetDeviceContainer staDevices;
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));
    staDevices = wifi.Install(phy, mac, wifiStaNodes);

    NetDeviceContainer apDevices;
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    apDevices = wifi.Install(phy, mac, wifiApNode);

    MobilityHelper mobility;

    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(0.0),
                                  "MinY",
                                  DoubleValue(0.0),
                                  "DeltaX",
                                  DoubleValue(5.0),
                                  "DeltaY",
                                  DoubleValue(10.0),
                                  "GridWidth",
                                  UintegerValue(20),
                                  "LayoutType",
                                  StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds",
                              RectangleValue(Rectangle(-100, 100, -100, 100))); // Mobilidad random para las STAs
    mobility.Install(wifiStaNodes);

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel"); // Posición fija para el AP
    mobility.Install(wifiApNode);

    InternetStackHelper stack;
    stack.Install(wifiApNode);
    stack.Install(wifiStaNodes);

    Ipv4AddressHelper address;

    address.SetBase("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer wifiInterfaces;
    wifiInterfaces = address.Assign(staDevices);
    address.Assign(apDevices);

    UdpEchoServerHelper echoServer(23);

    ApplicationContainer serverApps = echoServer.Install(wifiStaNodes.Get(0));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(wifiInterfaces.GetAddress(0), 23);
    echoClient.SetAttribute("MaxPackets", UintegerValue(nPkts));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(packetSize));

    ApplicationContainer clientApps = echoClient.Install(wifiStaNodes.Get(nSTAs - 1));
    clientApps.Start(Seconds(0.0));
    clientApps.Stop(Seconds(simulationTime));

    Simulator::Stop(Seconds(simulationTime));

    if (tracing)
    {
        phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
        phy.EnablePcap("third", apDevices.Get(0));
    }

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}


/* PRUEBAS
./ns3 run scratch/first-network
./ns3 run "scratch/first-network --nSTAs=5"
./ns3 run "scratch/first-network --nPkts=10 --nSTAs=3"
./ns3 run "scratch/first-network --nPkts=5 --nSTAs=2 --packetSize=4096"
./ns3 run "scratch/first-network --nPkts=5 --nSTAs=2 --packetSize=2048 --verbose=true"
*/