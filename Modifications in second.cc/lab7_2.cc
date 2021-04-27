#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"

//                    Network Topology
//
//       10.1.1.0                              10.1.3.0
// n0 -------------- n1   n2   n3   n4   n5 -------------- n6
//    point-to-point  |    |    |    |    | point-to-point
//                    =====================
//                        LAN 10.1.2.0


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SecondScriptExampleModified");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nCsma = 3;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);

  cmd.Parse (argc,argv);

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  nCsma = nCsma == 0 ? 1 : nCsma;

	/******************************************************************/

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

	NodeContainer p2pNodes1;
  p2pNodes1.Create (2);

  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1)); 
	csmaNodes.Add (p2pNodes1.Get (1));
  csmaNodes.Create (nCsma);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);
	
	PointToPointHelper pointToPoint1;
  pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint1.SetChannelAttribute ("Delay", StringValue ("2ms"));

	NetDeviceContainer p2pDevices1;
  p2pDevices1 = pointToPoint1.Install (p2pNodes1);

  CsmaHelper csma; // LAN has more bandwidth and lesser delay
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

	/******************************************************************/

  InternetStackHelper stack;
  stack.Install (p2pNodes.Get (0));
	stack.Install (p2pNodes1.Get (0));
  stack.Install (csmaNodes);

	/******************************************************************/

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

	address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces1;
  p2pInterfaces1 = address.Assign (p2pDevices1);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

	/******************************************************************/

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (p2pNodes.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));
  
  UdpEchoClientHelper echoClient (p2pInterfaces.GetAddress (0), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (p2pNodes1.Get (0));
  
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (4.0));

	/******************************************************************/

	//Populate all the routing tables to all the nodes.
	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	/******************************************************************/

  pointToPoint.EnablePcapAll ("lab7_2_p2p");
	pointToPoint1.EnablePcapAll ("lab7_2_p2p1");
  csma.EnablePcapAll ("lab7_2_csma");

	/******************************************************************/

  AnimationInterface anim("lab7_2.xml");
  anim.SetConstantPosition(p2pNodes.Get(0),10.0,10.0);
  anim.SetConstantPosition(p2pNodes.Get(1),20.0,20.0);
  anim.SetConstantPosition(csmaNodes.Get(2),30.0,20.0);
  anim.SetConstantPosition(csmaNodes.Get(3),40.0,20.0);
  anim.SetConstantPosition(csmaNodes.Get(4),50.0,20.0);
  anim.SetConstantPosition(p2pNodes1.Get(1),60.0,20.0);
	anim.SetConstantPosition(p2pNodes1.Get(0),70.0,10.0);

	/******************************************************************/

  AsciiTraceHelper ascii;
  pointToPoint.EnableAsciiAll(ascii.CreateFileStream("lab7_2_p2p.tr"));
  csma.EnableAsciiAll(ascii.CreateFileStream("lab7_2_csma.tr"));
  pointToPoint.EnableAsciiAll(ascii.CreateFileStream("lab7_2_p2p1.tr"));  

	/******************************************************************/

	Simulator::Run ();
  Simulator::Destroy ();

	/******************************************************************/

  return 0;
}


