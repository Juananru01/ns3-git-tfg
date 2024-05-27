// The simulation assumes a configurable number of stations in an infrastructure network:
//
//  STA     AP
//    *     *
//    |     |
//   n1     n2
//

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/he-phy.h"

NS_LOG_COMPONENT_DEFINE("he-wifi-network");

using namespace ns3;

double totalBytesReceived = 0;
double throughputInterval = 1;        //seconds
double simulationTime = 25;     //seconds
double totalThroughput = 0;
double frequency = 5;       // whether 2.4, 5 or 6 GHz
int timeInitCountMeanThroughput = 5;
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

    if(!macHeader.IsCtl()){
        totalBytesReceived += packet->GetSize();
    }
}

int main(int argc, char* argv[])
{
    bool tracing = false;
    double distance = 0.0;      // meters
    int channelWidth = 160;      // 20, 40, 80 o 160 Mhz
    uint32_t payloadSize = 700; // must fit in the max TX duration when transmitting at MCS 0 over an RU of 26 tones
    int mcs = 0;
    int gi = 800; // guard interval in ns (800, 1600 o 3200)
    std::string ossControlModeString = "";

    CommandLine cmd(__FILE__);
    cmd.AddValue("payloadSize", "The application payload size in bytes", payloadSize);
    cmd.AddValue("simulationTime", "Simulation time in seconds", simulationTime);
    cmd.AddValue("throughputInterval", "Throughput print interval in seconds", throughputInterval);
    cmd.AddValue("tracing", "Enable pcap tracing", tracing);
    cmd.AddValue("distance", "Distance in meters between the station and the access point", distance);
    cmd.AddValue("frequency", "Whether working in the 2.4, 5 or 6 GHz band (other values gets rejected)", frequency);
    cmd.AddValue("mcs", "if set, limit testing to a specific MCS (0-11)", mcs);
    cmd.Parse(argc, argv);

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ax);
    //wifiHelper.SetStandard(WIFI_STANDARD_80211be);
    std::string channelStr("{0, " + std::to_string(channelWidth) + ", ");
    StringValue ctrlRate;
    auto nonHtRefRateMbps = HePhy::GetNonHtReferenceRate(mcs) / 1e6;
    auto codeRate = HePhy::GetCodeRate(mcs);

    std::ostringstream ossDataMode;
    ossDataMode << "HeMcs" << mcs;

    if (frequency == 6)
    {
        wifi.SetStandard(WIFI_STANDARD_80211ax);
        ctrlRate = StringValue(ossDataMode.str());
        channelStr += "BAND_6GHZ, 0}";
        Config::SetDefault("ns3::LogDistancePropagationLossModel::ReferenceLoss",
                            DoubleValue(48));
    }
    else if (frequency == 5)
    {
        wifi.SetStandard(WIFI_STANDARD_80211ax);
        std::ostringstream ossControlMode;
        ossControlMode << "OfdmRate" << nonHtRefRateMbps << "Mbps";
        ossControlModeString = ossControlMode.str();
        ctrlRate = StringValue(ossControlMode.str());
        channelStr += "BAND_5GHZ, 0}";
        Config::SetDefault("ns3::LogDistancePropagationLossModel::ReferenceLoss",
                            DoubleValue(46.4));
    }
    else if (frequency == 2.4)
    {
        wifi.SetStandard(WIFI_STANDARD_80211ax);
        std::ostringstream ossControlMode;
        ossControlMode << "ErpOfdmRate" << nonHtRefRateMbps << "Mbps";
        ctrlRate = StringValue(ossControlMode.str());
        channelStr += "BAND_2_4GHZ, 0}";
        Config::SetDefault("ns3::LogDistancePropagationLossModel::ReferenceLoss",
                            DoubleValue(40));
    }
    else
    {
        std::cout << "Wrong frequency value!" << std::endl;
        return 0;
    }

    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                    "DataMode",
                                    StringValue(ossDataMode.str()),
                                    "ControlMode",
                                    ctrlRate);

    // Set guard interval
    wifi.ConfigHeOptions("GuardInterval", TimeValue(NanoSeconds(gi)));

    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();

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
    apDevice = wifi.Install(wifiPhy, wifiMac, apWifiNode);

    /* Configure STA */
    wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));

    NetDeviceContainer staDevices;
    staDevices = wifi.Install(wifiPhy, wifiMac, staWifiNode);

    // Mobility
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();

    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(distance, 0.0, 0.0));
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
    ApplicationContainer sinkApp = sinkHelper.Install(staWifiNode);

    const auto maxLoad = HePhy::GetDataRate(mcs, channelWidth, gi, 1);
    const auto packetInterval = payloadSize * 8.0 / maxLoad;
    
    /* Install UDP Transmitter on the station */
    UdpClientHelper client(staInterface.GetAddress(0), 9);
    client.SetAttribute("MaxPackets", UintegerValue(0));
    client.SetAttribute("Interval", TimeValue(Seconds(packetInterval)));
    client.SetAttribute("PacketSize", UintegerValue(payloadSize));
    ApplicationContainer serverApp = client.Install(apWifiNode);

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

    double averageThroughput = totalThroughput / numIntervals;
    std::cout << "************** Throughput medio simulación: " << averageThroughput << " Mbps, Distance: "<< distance << " meters, MCS: "<< ossDataMode.str() << ", CtrlRate: " << ossControlModeString << ", Simulation Time: " << simulationTime-timeInitCountMeanThroughput << " seconds" << " ****************" << std::endl;

    Simulator::Destroy();

    return 0;
}
