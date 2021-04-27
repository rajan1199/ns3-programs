 #include "ns3/core-module.h"
 #include "ns3/network-module.h"
 #include "ns3/netanim-module.h"
 #include "ns3/internet-module.h"
 #include "ns3/point-to-point-module.h"
 #include "ns3/applications-module.h"
 #include "ns3/point-to-point-layout-module.h"
            
 //
 // Network Topology
 // Star Toloplogy with 1 server and 6 hosts
 // Point-to-point connections with UDP 
 
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstTopology");

int main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (7);
	
	/**********************************************************/

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

	PointToPointHelper pointToPoint1;
  pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint1.SetChannelAttribute ("Delay", StringValue ("2ms"));

	PointToPointHelper pointToPoint2;
  pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint2.SetChannelAttribute ("Delay", StringValue ("2ms"));

	PointToPointHelper pointToPoint3;
  pointToPoint3.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint3.SetChannelAttribute ("Delay", StringValue ("2ms"));

	PointToPointHelper pointToPoint4;
  pointToPoint4.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint4.SetChannelAttribute ("Delay", StringValue ("2ms"));

	PointToPointHelper pointToPoint5;
  pointToPoint5.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint5.SetChannelAttribute ("Delay", StringValue ("2ms"));

	/**********************************************************/

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes.Get(1), nodes.Get(0));

  NetDeviceContainer devices1;
  devices1 = pointToPoint1.Install (nodes.Get(2), nodes.Get(0));

  NetDeviceContainer devices2;
  devices2 = pointToPoint2.Install (nodes.Get(3), nodes.Get(0));

  NetDeviceContainer devices3;
  devices3 = pointToPoint3.Install (nodes.Get(4), nodes.Get(0));

  NetDeviceContainer devices4;
  devices4 = pointToPoint4.Install (nodes.Get(5), nodes.Get(0));

  NetDeviceContainer devices5;
  devices5 = pointToPoint5.Install (nodes.Get(6), nodes.Get(0));

	/**********************************************************/

  InternetStackHelper stack;
  stack.Install (nodes);

	/**********************************************************/

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4AddressHelper address1;
  address1.SetBase ("10.1.2.0", "255.255.255.0");

  Ipv4AddressHelper address2;
  address2.SetBase ("10.1.3.0", "255.255.255.0");

  Ipv4AddressHelper address3;
  address3.SetBase ("10.1.4.0", "255.255.255.0");

  Ipv4AddressHelper address4;
  address4.SetBase ("10.1.5.0", "255.255.255.0");

  Ipv4AddressHelper address5;
  address5.SetBase ("10.1.6.0", "255.255.255.0");

	/**********************************************************/

  Ipv4InterfaceContainer interfaces = address.Assign (devices);
  Ipv4InterfaceContainer interfaces1 = address1.Assign (devices1);
  Ipv4InterfaceContainer interfaces2 = address2.Assign (devices2);
  Ipv4InterfaceContainer interfaces3 = address3.Assign (devices3);
  Ipv4InterfaceContainer interfaces4 = address4.Assign (devices4);
  Ipv4InterfaceContainer interfaces5 = address5.Assign (devices5);

	/**********************************************************/
                                                                       
  UdpEchoServerHelper echoServer (9);
  ApplicationContainer serverApps = echoServer.Install (nodes.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (50.0));

	/**********************************************************/

  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  UdpEchoClientHelper echoClient1 (interfaces1.GetAddress (1), 9);
  echoClient1.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient1.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient1.SetAttribute ("PacketSize", UintegerValue (1024));

  UdpEchoClientHelper echoClient2 (interfaces2.GetAddress (1), 9);
  echoClient2.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient2.SetAttribute ("PacketSize", UintegerValue (1024));

  UdpEchoClientHelper echoClient3 (interfaces3.GetAddress (1), 9);
  echoClient3.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient3.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient3.SetAttribute ("PacketSize", UintegerValue (1024));

  UdpEchoClientHelper echoClient4 (interfaces4.GetAddress (1), 9);
  echoClient4.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient4.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient4.SetAttribute ("PacketSize", UintegerValue (1024));

  UdpEchoClientHelper echoClient5 (interfaces5.GetAddress (1), 9);
  echoClient5.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient5.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient5.SetAttribute ("PacketSize", UintegerValue (1024));

	/**********************************************************/

  ApplicationContainer clientApps = echoClient.Install(nodes.Get (1));
  ApplicationContainer clientApps1 = echoClient1.Install(nodes.Get (2));
  ApplicationContainer clientApps2 = echoClient2.Install(nodes.Get (3));
  ApplicationContainer clientApps3 = echoClient3.Install(nodes.Get (4));
  ApplicationContainer clientApps4 = echoClient4.Install(nodes.Get (5));
  ApplicationContainer clientApps5 = echoClient5.Install(nodes.Get (6));

	/**********************************************************/

  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (4.0));

  clientApps1.Start (Seconds (5.0));
  clientApps1.Stop (Seconds (7.0));

  clientApps2.Start (Seconds (8.0));
  clientApps2.Stop (Seconds (10.0));

  clientApps3.Start (Seconds (11.0));
  clientApps3.Stop (Seconds (13.0));

  clientApps4.Start (Seconds (14.0));
  clientApps4.Stop (Seconds (16.0));

  clientApps5.Start (Seconds (17.0));
  clientApps5.Stop (Seconds (19.0));

	/**********************************************************/

  pointToPoint.EnablePcapAll("channel01");
  pointToPoint1.EnablePcapAll("channel02");
  pointToPoint2.EnablePcapAll("channel03");
  pointToPoint3.EnablePcapAll("channel04");
  pointToPoint4.EnablePcapAll("channel05");
  pointToPoint5.EnablePcapAll("channel06");

	/**********************************************************/

  AnimationInterface anim("mystar.xml");
  anim.SetConstantPosition(nodes.Get(0),49.0,37.5);
  anim.SetConstantPosition(nodes.Get(1),0.0,0.0);
	anim.SetConstantPosition(nodes.Get(2),0.0,75.0);
	anim.SetConstantPosition(nodes.Get(3),40.0,75.0);
	anim.SetConstantPosition(nodes.Get(4),49.0,0.0);
	anim.SetConstantPosition(nodes.Get(5),98.0,0.0);
	anim.SetConstantPosition(nodes.Get(6),98.0,75.0);

	/**********************************************************/

  AsciiTraceHelper ascii;
  pointToPoint.EnableAsciiAll(ascii.CreateFileStream("p2pstar0.tr"));
	pointToPoint1.EnableAsciiAll(ascii.CreateFileStream("p2pstar1.tr"));
	pointToPoint2.EnableAsciiAll(ascii.CreateFileStream("p2pstar2.tr"));
	pointToPoint3.EnableAsciiAll(ascii.CreateFileStream("p2pstar3.tr"));
	pointToPoint4.EnableAsciiAll(ascii.CreateFileStream("p2pstar4.tr"));
	pointToPoint5.EnableAsciiAll(ascii.CreateFileStream("p2pstar5.tr"));

	/**********************************************************/

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
 
 }
