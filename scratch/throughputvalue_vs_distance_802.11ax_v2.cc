/*
 * Copyright (c) 2016 SEBASTIEN DERONNE
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Sebastien Deronne <sebastien.deronne@gmail.com>
 */

#include "ns3/boolean.h"
#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/double.h"
#include "ns3/enum.h"
#include "ns3/he-phy.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/log.h"
#include "ns3/mobility-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/ssid.h"
#include "ns3/string.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/udp-server.h"
#include "ns3/uinteger.h"
#include "ns3/wifi-acknowledgment.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"

#include <functional>

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
    std::cout << "Total MBytes received: " << totalBytesReceived / 1000000 << ", Total time: " << time << "s, Throughput: " << throughput << " Mbps" << std::endl;
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
    double distance = 0.0;      // meters
    double frequency = 5;       // whether 2.4, 5 or 6 GHz
    int mcs = 0;
    uint32_t payloadSize =
        700; // must fit in the max TX duration when transmitting at MCS 0 over an RU of 26 tones
    int channelWidth = 160; // 20, 40, 80, 160 Mhz
    int gi = 800; // guard interval in ns (800, 1600 o 3200)

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
    cmd.AddValue("mcs", "if set, limit testing to a specific MCS (0-11)", mcs);
    cmd.AddValue("payloadSize", "The application payload size in bytes", payloadSize);

    cmd.Parse(argc, argv);

    NodeContainer networkNodes;
    networkNodes.Create(2);
    Ptr<Node> apWifiNode = networkNodes.Get(0);
    Ptr<Node> staWifiNode = networkNodes.Get(1);

    NetDeviceContainer apDevice;
    NetDeviceContainer staDevice;
    WifiMacHelper mac;
    WifiHelper wifi;
    std::string channelStr("{0, " + std::to_string(channelWidth) + ", ");
    StringValue ctrlRate;
    auto nonHtRefRateMbps = HePhy::GetNonHtReferenceRate(mcs) / 1e6;

    std::ostringstream ossDataMode;
    ossDataMode << "HeMcs" << mcs;

    std::string ossControlModeString = "";

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
        ossControlModeString = ossControlMode.str();
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

    Ssid ssid = Ssid("ns3-80211ax");

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
    phy.SetChannel(channel.Create());

    mac.SetType("ns3::StaWifiMac",
                "Ssid",
                SsidValue(ssid));
    phy.Set("ChannelSettings", StringValue(channelStr));
    staDevice = wifi.Install(phy, mac, staWifiNode);

    mac.SetType("ns3::ApWifiMac",
                "EnableBeaconJitter",
                BooleanValue(false),
                "Ssid",
                SsidValue(ssid));
    apDevice = wifi.Install(phy, mac, apWifiNode);

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

    const auto maxLoad = HePhy::GetDataRate(mcs, channelWidth, gi, 1);

    // UDP flow
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

    Simulator::Schedule(Seconds(0), &Ipv4GlobalRoutingHelper::PopulateRoutingTables);

    Simulator::Stop(Seconds(simulationTime + throughputInterval));
    Simulator::Run();

    double averageThroughput = totalThroughput / numIntervals;
    std::cout << "************** Throughput medio simulación: " << averageThroughput << " Mbps, Distance: "<< distance << " meters, MCS: "<< ossDataMode.str() << ", CtrlRate: " << ossControlModeString << ", Simulation Time: " << simulationTime-timeInitCountMeanThroughput << " seconds" << " ****************" << std::endl;

    Simulator::Destroy();
}

