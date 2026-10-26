#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/point-to-point-module.h"
using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("CsmaSimulationExample");
int main (int argc, char *argv[])
{
	CommandLine cmd;
	cmd.Parse (argc, argv);
	NodeContainer nodes;
	nodes.Create (4);
	CsmaHelper csma;

	csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
	csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

	NetDeviceContainer devices;
	devices = csma.Install (nodes);
	InternetStackHelper stack;
	stack.Install (nodes);
	Ipv4AddressHelper address;
	
	address.SetBase ("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces = address.Assign (devices);
	UdpEchoServerHelper echoServer (9);
	ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
	
	serverApps.Start (Seconds (1.0));
	serverApps.Stop (Seconds (10.0));
	
	UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);

	echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
	echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
	echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

	ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
	clientApps.Start (Seconds (2.0));
	clientApps.Stop (Seconds (10.0));
	AsciiTraceHelper ascii;

	csma.EnableAsciiAll (ascii.CreateFileStream ("csma-simulation.tr"));
	csma.EnablePcap ("csma-simulation", devices.Get (1), true);
	AnimationInterface anim ("csma-simulation.xml");

	Simulator::Run ();
	Simulator::Destroy ();

	return 0;
}
