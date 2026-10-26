#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main(int argc, char *argv[])
{
	NodeContainer nodes;
	nodes.Create(2);
	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
	pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
	
	NetDeviceContainer devices;
	devices = pointToPoint.Install(nodes);
	InternetStackHelper stack;
	stack.Install(nodes);
	Ipv4AddressHelper address;
	address.SetBase("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces = address.Assign(devices);
	uint16_t port=9;
	UdpEchoServerHelper server(port);
	ApplicationContainer serverApps = server.Install(nodes.Get(1));
	serverApps.Start(Seconds(1.0));
	serverApps.Stop(Seconds(10.0));
	
	UdpEchoClientHelper client(interfaces.GetAddress(1), port);
	client.SetAttribute("MaxPackets", UintegerValue(1));
	client.SetAttribute("Interval", TimeValue(Seconds(1.0)));
	client.SetAttribute("PacketSize", UintegerValue(1024));
	
	ApplicationContainer ClientApps = client.Install(nodes.Get(0));
	ClientApps.Start(Seconds(2.0));
	ClientApps.Stop(Seconds(10.0));
	
	pointToPoint.EnableAsciiAll("p2p-trace");	
	
	AnimationInterface anim("p2p-animation.xml");
	Simulator::Run ();
  	Simulator::Destroy ();
	return 0;
}

