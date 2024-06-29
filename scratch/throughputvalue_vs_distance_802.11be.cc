#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/eht-phy.h"
#include "ns3/multi-model-spectrum-channel.h"
#include "ns3/spectrum-wifi-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/rng-seed-manager.h"

// The simulation assumes a configurable number of stations in an infrastructure network:
//
//  STA     AP
//    *     *
//    |     |
//   n1     n2
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("eht-wifi-network");

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
    double frequency = 5;       // whether 2.4, 5 or 6 GHz
    double distance = 0.0;      // meters
    int mcs = 0;
    uint32_t payloadSize =
        700; // must fit in the max TX duration when transmitting at MCS 0 over an RU of 26 tones
    int channelWidth = 160; // 20, 40, 80, 160, 320 Mhz
    int gi = 800; // guard interval in ns (800, 1600 o 3200)
    uint32_t seed = 1; // semilla

    CommandLine cmd(__FILE__);
    cmd.AddValue("frequency",
                 "Whether working in the 2.4, 5 or 6 GHz band (other values gets rejected)",
                 frequency);
    cmd.AddValue("distance",
                 "Distance in meters between the station and the access point",
                 distance);
    cmd.AddValue("simulationTime", "Simulation time in seconds", simulationTime);
    cmd.AddValue("mcs", "if set, limit testing to a specific MCS (0-11)", mcs);
    cmd.AddValue("guard_interval",
                 "Guard interval (800, 1600, 3200) in ns",
                 gi);
    cmd.AddValue("channelWidth",
                 "Channel width in MHz (20, 40, 80, 160)",
                 channelWidth);
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
    FrequencyRange freqRange;

    wifi.SetStandard(WIFI_STANDARD_80211be);
    std::string dataModeStr = "EhtMcs" + std::to_string(mcs);
    std::string ctrlRateStr;
    uint64_t nonHtRefRateMbps = EhtPhy::GetNonHtReferenceRate(mcs) / 1e6;

    if (frequency == 6)
    {
        channelStr += "BAND_6GHZ, 0}";
        freqRange = WIFI_SPECTRUM_6_GHZ;
        Config::SetDefault("ns3::LogDistancePropagationLossModel::ReferenceLoss",
                            DoubleValue(48));

        wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                            "DataMode",
                            StringValue(dataModeStr));
    }
    else if (frequency == 5)
    {
        channelStr += "BAND_5GHZ, 0}";
        freqRange = WIFI_SPECTRUM_5_GHZ;
        ctrlRateStr = "OfdmRate" + std::to_string(nonHtRefRateMbps) + "Mbps";

        wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                            "DataMode",
                            StringValue(dataModeStr),
                            "ControlMode",
                            StringValue(ctrlRateStr));

    }
    else if (frequency == 2.4)
    {
        channelStr += "BAND_2_4GHZ, 0}";
        freqRange = WIFI_SPECTRUM_2_4_GHZ;
        Config::SetDefault("ns3::LogDistancePropagationLossModel::ReferenceLoss",
                            DoubleValue(40));
        ctrlRateStr = "ErpOfdmRate" + std::to_string(nonHtRefRateMbps) + "Mbps";

        wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                            "DataMode",
                            StringValue(dataModeStr),
                            "ControlMode",
                            StringValue(ctrlRateStr));
    }

    // Set guard interval
    wifi.ConfigHeOptions("GuardInterval", TimeValue(NanoSeconds(gi)));

    Ssid ssid = Ssid("ns3-80211be");


    SpectrumWifiPhyHelper wifiPhy;
    wifiPhy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);

    wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
    wifiPhy.Set("ChannelSettings", StringValue(channelStr));

    auto spectrumChannel = CreateObject<MultiModelSpectrumChannel>();
    auto lossModel = CreateObject<LogDistancePropagationLossModel>();
    spectrumChannel->AddPropagationLossModel(lossModel);
    wifiPhy.AddChannel(spectrumChannel, freqRange);

    staDevice = wifi.Install(wifiPhy, wifiMac, staWifiNode);

    wifiMac.SetType("ns3::ApWifiMac",
                "EnableBeaconJitter",
                BooleanValue(false),
                "Ssid",
                SsidValue(ssid));
    apDevice = wifi.Install(wifiPhy, wifiMac, apWifiNode);

    RngSeedManager::SetSeed(seed);
    RngSeedManager::SetRun(1);

    // Mobility.
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

    const auto maxLoad = EhtPhy::GetDataRate(mcs, channelWidth, gi, 1);

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
    if(frequency == 6) {
        std::cout << "************** Throughput medio simulación: " << averageThroughput << " Mbps, Distance: "<< distance << " meters, MCS: "<< dataModeStr << ", Simulation Time: " << simulationTime-timeInitCountMeanThroughput << " seconds" << " ****************" << std::endl;
    } else {
        std::cout << "************** Throughput medio simulación: " << averageThroughput << " Mbps, Distance: "<< distance << " meters, MCS: "<< dataModeStr << ", CtrlRate: " << ctrlRateStr << ", Simulation Time: " << simulationTime-timeInitCountMeanThroughput << " seconds" << " ****************" << std::endl;
    }

    Simulator::Destroy();

    return 0;
}


