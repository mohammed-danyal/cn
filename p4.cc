#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/netanim-module.h"
using namespace ns3;
NS_LOG_COMPONENT_DEFINE("BusTopologyExample");
int main(int argc, char *argv[]) {
CommandLine cmd;
cmd.Parse(argc, argv);
// Create 5 nodes
NodeContainer nodes;
nodes.Create(5);
// Point-to-Point Helper
PointToPointHelper pointToPoint;
pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
pointToPoint.SetChannelAttribute("Delay", TimeValue(NanoSeconds(100)));
// Connect nodes in a linear topology
NetDeviceContainer devices01 = pointToPoint.Install(nodes.Get(0), nodes.Get(1));
NetDeviceContainer devices12 = pointToPoint.Install(nodes.Get(1), nodes.Get(2));
NetDeviceContainer devices23 = pointToPoint.Install(nodes.Get(2), nodes.Get(3));
NetDeviceContainer devices34 = pointToPoint.Install(nodes.Get(3), nodes.Get(4));
// Install Internet stack
InternetStackHelper stack;
stack.Install(nodes);
// Assign IP addresses to each link
Ipv4AddressHelper address;
address.SetBase("10.1.1.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces01 = address.Assign(devices01);
address.SetBase("10.1.2.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces12 = address.Assign(devices12);
address.SetBase("10.1.3.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces23 = address.Assign(devices23);
address.SetBase("10.1.4.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces34 = address.Assign(devices34);
// Populate routing tables
Ipv4GlobalRoutingHelper::PopulateRoutingTables();
// Install UDP server on Node 1
UdpServerHelper udpServer(9);
ApplicationContainer serverApp = udpServer.Install(nodes.Get(1));
serverApp.Start(Seconds(1.0));
serverApp.Stop(Seconds(10.0));
// Get correct IP address of Node 1 (second device of devices01)
Ipv4Address serverAddress = interfaces01.GetAddress(1);
// Install UDP client on Node 0 targeting Node 1
UdpClientHelper udpClient(serverAddress, 9);
udpClient.SetAttribute("MaxPackets", UintegerValue(320));
udpClient.SetAttribute("Interval", TimeValue(MilliSeconds(50)));
udpClient.SetAttribute("PacketSize", UintegerValue(1024));
ApplicationContainer clientApp = udpClient.Install(nodes.Get(0));
clientApp.Start(Seconds(2.0));
clientApp.Stop(Seconds(10.0));
// Enable pcap tracing
pointToPoint.EnablePcapAll("bus-topology");
// Enable ASCII tracing for analysis
AsciiTraceHelper ascii;
pointToPoint.EnableAsciiAll(ascii.CreateFileStream("bus-topology.tr"));
// Enable logs
LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
// NetAnim visualization
AnimationInterface anim("bus-topology.xml");
// Set constant positions for visualization
anim.SetConstantPosition(nodes.Get(0), 100, 100);
anim.SetConstantPosition(nodes.Get(1), 200, 100);
anim.SetConstantPosition(nodes.Get(2), 300, 100);
anim.SetConstantPosition(nodes.Get(3), 400, 100);
anim.SetConstantPosition(nodes.Get(4), 500, 100);
Simulator::Run();
Simulator::Destroy();
return 0;
}
