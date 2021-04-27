#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"

//              Network Topology
//
//                           10.1.1.0
// n5   n4   n3   n2   n0----------------n1   n6   n7
// |    |    |    |    |                 |    |    |  
// =====================                 ===========
//      LAN 10.1.2.0                     LAN 10.1.3.0


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SecondScriptExample");


int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nCsma1 = 4;
	uint32_t nCsma2 = 2;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma1);
	cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma2);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);

  cmd.Parse (argc,argv);

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

	/******************************************************************/

  nCsma1 = nCsma1 == 0 ? 1 : nCsma1;
	nCsma2 = nCsma2 == 0 ? 1 : nCsma2;

	/******************************************************************/

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  NodeContainer csmaNodes1;
  csmaNodes1.Add (p2pNodes.Get (0)); 
  csmaNodes1.Create (nCsma1); 

	NodeContainer csmaNodes2;
  csmaNodes2.Add (p2pNodes.Get (1)); 
  csmaNodes2.Create (nCsma2); 

	/******************************************************************/

	PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

	CsmaHelper csma1; // LAN has more bandwidth and lesser delay
  csma1.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma1.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices1;
  csmaDevices1 = csma1.Install (csmaNodes1);

	CsmaHelper csma2; // LAN has more bandwidth and lesser delay
  csma2.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma2.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices2;
  csmaDevices2 = csma2.Install (csmaNodes2);

	/******************************************************************/

  InternetStackHelper stack;
  stack.Install (csmaNodes1);	
  stack.Install (csmaNodes2);

	/******************************************************************/

  Ipv4AddressHelper address;
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

	address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces1;
  csmaInterfaces1 = address.Assign (csmaDevices1);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces2;
  csmaInterfaces2 = address.Assign (csmaDevices2);

	/******************************************************************/

  UdpEchoServerHelper echoServer (9);
  ApplicationContainer serverApps = echoServer.Install (csmaNodes1.Get (nCsma1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

	UdpEchoClientHelper echoClient (csmaInterfaces1.GetAddress (nCsma1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (csmaNodes2.Get (nCsma2));
  
	//n0 is the client in this topology
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (4.0));

	/******************************************************************/

	//Populate all the routing tables to all the nodes.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	/******************************************************************/

  pointToPoint.EnablePcapAll ("lab7_3_p2p");
	csma1.EnablePcapAll ("lab7_3_csma1");
  csma2.EnablePcapAll ("lab7_3_csma2");

	/******************************************************************/

  AnimationInterface anim("lab7_3.xml");
	anim.SetConstantPosition(csmaNodes1.Get(0),50.0,30.0);
  anim.SetConstantPosition(csmaNodes1.Get(1),40.0,30.0);
  anim.SetConstantPosition(csmaNodes1.Get(2),30.0,30.0);
  anim.SetConstantPosition(csmaNodes1.Get(3),20.0,30.0);
  anim.SetConstantPosition(csmaNodes1.Get(4),10.0,30.0);

	anim.SetConstantPosition(csmaNodes2.Get(0),60.0,30.0);
  anim.SetConstantPosition(csmaNodes2.Get(1),70.0,30.0);
  anim.SetConstantPosition(csmaNodes2.Get(2),80.0,30.0);

	/******************************************************************/

  AsciiTraceHelper ascii;
  pointToPoint.EnableAsciiAll(ascii.CreateFileStream("lab7_3_p2p.tr"));
  csma1.EnableAsciiAll(ascii.CreateFileStream("lab7_3_csma1.tr"));
  csma2.EnableAsciiAll(ascii.CreateFileStream("lab7_3_csma2.tr"));  

	/******************************************************************/

	Simulator::Run ();
  Simulator::Destroy ();

	/******************************************************************/
  return 0;
}
  








