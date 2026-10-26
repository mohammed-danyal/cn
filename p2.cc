#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
using namespace ns3;
int main (int argc, char *argv[])
{
// Create nodes
NodeContainer nodes;
nodes.Create (3);
// Configure P2P channels
PointToPointHelper pointToPoint;
pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
// Install devices on the nodes
NetDeviceContainer devices1, devices2;
devices1 = pointToPoint.Install (nodes.Get(0), nodes.Get(1));
devices2 = pointToPoint.Install (nodes.Get(1), nodes.Get(2));
// Install internet stack
InternetStackHelper stack;
stack.Install (nodes);
// Assign IP addresses
Ipv4AddressHelper address;
address.SetBase ("10.1.1.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces1 = address.Assign (devices1);
address.SetBase ("10.1.2.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces2 = address.Assign (devices2);
// Set up server on central node
uint16_t port = 9; // Discard port (RFC 863)
UdpEchoServerHelper server (port);
ApplicationContainer serverApp = server.Install (nodes.Get(1));
serverApp.Start (Seconds (1.0));
serverApp.Stop (Seconds (10.0));
// Set up clients
UdpEchoClientHelper client1 (interfaces1.GetAddress (1), port);
client1.SetAttribute ("MaxPackets", UintegerValue (1));
client1.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
client1.SetAttribute ("PacketSize", UintegerValue (1024));
ApplicationContainer clientApps1 = client1.Install (nodes.Get(0));
clientApps1.Start (Seconds (2.0));
clientApps1.Stop (Seconds (10.0));
UdpEchoClientHelper client2 (interfaces2.GetAddress (1), port);
client2.SetAttribute ("MaxPackets", UintegerValue (1));
client2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
client2.SetAttribute ("PacketSize", UintegerValue (1024));
ApplicationContainer clientApps2 = client2.Install (nodes.Get(2));
clientApps2.Start (Seconds (2.5));
clientApps2.Stop (Seconds (10.0));
// Enable Ascii tracing
pointToPoint.EnableAsciiAll ("p2p-trace");
// Set up NetAnim animation
AnimationInterface anim ("p2p-animation.xml");
// Run the simulation
Simulator::Run ();
Simulator::Destroy ();
return 0;
}
