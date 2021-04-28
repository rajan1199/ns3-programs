/*
*
* Network topology:
*
*   Ap    STA
*   *      *
*   |      |
*   n1     n2
*
*/

#include "ns3/config.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-model.h"
#include "ns3/packet-sink.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/tcp-westwood.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"

NS_LOG_COMPONENT_DEFINE("Friis_Propagation_Loss_Model");

using namespace ns3;

Ptr<PacketSink> sink;     /* Pointer to the packet sink application */
uint64_t lastTotalRx = 0; /* The value of the last total received bytes */

void CalculateThroughput()
{
    /************************************************ Return the simulator's virtual time*************************************/
    Time now = Simulator::Now();                                       
    
    /**************************************** Convert Application RX Packets to MBits. ***************************************/
    double cur = (sink->GetTotalRx() - lastTotalRx) * (double)8 / 1e5;
    std::cout << now.GetSeconds() << "s: \t" << cur << " Mbit/s" << std::endl;
    
    lastTotalRx = sink->GetTotalRx();
    Simulator::Schedule(MilliSeconds(100), &CalculateThroughput);
}

int main(int argc, char *argv[])
{
    std::cout << "Using FriisPropagationLossModel \n" << std::endl;

    uint32_t payloadSize = 1472;           /* Transport layer payload size in bytes. */
    std::string dataRate = "100Mbps";      /* Application layer datarate. */
    std::string tcpVariant = "TcpNewReno"; /* TCP variant type. */
    std::string phyRate = "HtMcs7";        /* Physical layer bitrate. */
    double simulationTime = 5;            /* Simulation time in seconds. */
    
    // LogComponentEnable("OnOffApplication", LOG_INFO);
    // LogComponentEnable("PacketSink", LOG_INFO);
    // LogComponentEnable("WifiMac", LOG_INFO);
    // LogComponentEnable("WifiPhy", LOG_INFO);

    tcpVariant = std::string("ns3::") + tcpVariant;
    TypeId tcpTid;
    NS_ABORT_MSG_UNLESS(TypeId::LookupByNameFailSafe(tcpVariant, &tcpTid), "TypeId " << tcpVariant << " not found");
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TypeId::LookupByName(tcpVariant)));

    /************************************* Configure TCP Options **********************************************************/
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(payloadSize));

    WifiMacHelper wifiMac;
    WifiHelper wifiHelper;
    wifiHelper.SetStandard(WIFI_STANDARD_80211n_2_4GHZ);

    /*************************************************** Set up Legacy Channel ***********************************************/
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");

    /************************************************** Setup Physical Layer *************************************************/
    YansWifiPhyHelper wifiPhy;
    wifiPhy.SetChannel(wifiChannel.Create());
    wifiPhy.SetErrorRateModel("ns3::YansErrorRateModel");
    wifiHelper.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                       "DataMode", StringValue(phyRate),
                                       "ControlMode", StringValue("HtMcs0"));

    /************************************************** Create Nodes ********************************************************/
    NodeContainer networkNodes;
    networkNodes.Create(2);
    Ptr<Node> apWifiNode = networkNodes.Get(0);
    Ptr<Node> staWifiNode = networkNodes.Get(1);

    /************************************************** Configure Access Point (AP)********************************************/
    Ssid ssid = Ssid("network");
    wifiMac.SetType("ns3::ApWifiMac",
                    "Ssid", SsidValue(ssid));

    NetDeviceContainer apDevice;
    apDevice = wifiHelper.Install(wifiPhy, wifiMac, apWifiNode);

    /******************************************************** Configure Station Node (STA)*************************************/
    wifiMac.SetType("ns3::StaWifiMac",
                    "Ssid", SsidValue(ssid));

    NetDeviceContainer staDevices;
    staDevices = wifiHelper.Install(wifiPhy, wifiMac, staWifiNode);

    /************************************************************** Mobility model ********************************************/
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(100.0, 100.0, 0.0));

    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(apWifiNode);
    mobility.Install(staWifiNode);

    /************************************************* Internet stack *********************************************************/
    InternetStackHelper stack;
    stack.Install(networkNodes);

    Ipv4AddressHelper address;
    address.SetBase("10.0.0.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterface;
    apInterface = address.Assign(apDevice);
    Ipv4InterfaceContainer staInterface;
    staInterface = address.Assign(staDevices);

    /* Populate routing table */
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    /******************************************** Install TCP Receiver on the access point ************************************/
    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), 9));
    ApplicationContainer sinkApp = sinkHelper.Install(apWifiNode);
    sink = StaticCast<PacketSink>(sinkApp.Get(0));

    /********************************************* Install TCP/UDP Transmitter on the station**********************************/
    OnOffHelper server("ns3::TcpSocketFactory", (InetSocketAddress(apInterface.GetAddress(0), 9)));
    server.SetAttribute("PacketSize", UintegerValue(payloadSize));
    server.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    server.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    server.SetAttribute("DataRate", DataRateValue(DataRate(dataRate)));
    ApplicationContainer serverApp = server.Install(staWifiNode);

    /******************************************************** Start Applications ********************************************/
    sinkApp.Start(Seconds(0.0));
    serverApp.Start(Seconds(1.0));
    Simulator::Schedule(Seconds(1.1), &CalculateThroughput);

/******************************************************Enable Traces*********************************************************/


    wifiPhy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
    wifiPhy.EnablePcap("Friis_AP", apDevice);
    wifiPhy.EnablePcap("Friis_STA", staDevices);

    // AsciiTraceHelper ascii;
    // WifiPhy.EnableAsciiAll(ascii.CreateFileStream("trace1.tr"));

    AsciiTraceHelper ascii;
    Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream("Friis.tr");
    wifiPhy.EnableAsciiAll (stream);

    AnimationInterface anim("Friis.xml");
    AnimationInterface::SetConstantPosition(apWifiNode, 0.0, 0.0);
    AnimationInterface::SetConstantPosition(staWifiNode, 100.0, 100.0);
    
 /*******************************************FlowMonitor******************************************************************/
 Ptr<FlowMonitor> flowMonitor;
 FlowMonitorHelper flowHelper;
 flowMonitor = flowHelper.InstallAll();

    /***************************************************** Start Simulation ***************************************************/
    Simulator::Stop(Seconds(simulationTime + 1));
    Simulator::Run();

/***********************create xml file from flowmonitor***********************************************************************/
flowMonitor->SerializeToXmlFile("flowFriss.xml", true, true);

    double averageThroughput = ((sink->GetTotalRx() * 8) / (1e6 * simulationTime));

    Simulator::Destroy();

    // if (averageThroughput < 50)
    // {
    //     NS_LOG_ERROR("Obtained throughput is not in the expected boundaries!");
    //     exit(1);
    // }

    std::cout << "\nAverage throughput: " << averageThroughput << " Mbit/s" << std::endl;
    return 0;
}
