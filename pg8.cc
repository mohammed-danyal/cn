#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TcpCongestion");

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
    *stream->GetStream () << Simulator::Now ().GetSeconds ()
                          << "\t" << newCwnd << std::endl;

    std::cout << "Time: "
              << Simulator::Now ().GetSeconds ()
              << "s  CWND: "
              << newCwnd
              << " bytes" << std::endl;
}

int
main (int argc, char *argv[])
{
    uint32_t packetSize = 1040;
    double simulationTime = 20.0;

    CommandLine cmd;
    cmd.Parse (argc, argv);

    // Create nodes
    NodeContainer nodes;
    nodes.Create (2);

    // Point-to-point link
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate",
                                     StringValue ("2Mbps"));
    pointToPoint.SetChannelAttribute ("Delay",
                                      StringValue ("10ms"));

    NetDeviceContainer devices;
    devices = pointToPoint.Install (nodes);

    // Install Internet stack
    InternetStackHelper internet;
    internet.Install (nodes);

    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0",
                     "255.255.255.0");

    Ipv4InterfaceContainer interfaces;
    interfaces = address.Assign (devices);

    // Use TCP NewReno
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType",
                        TypeIdValue (TcpNewReno::GetTypeId ()));

    // Create TCP sink
    uint16_t port = 5000;

    Address sinkAddress (
        InetSocketAddress (interfaces.GetAddress (1), port));

    PacketSinkHelper sinkHelper (
        "ns3::TcpSocketFactory",
        InetSocketAddress (Ipv4Address::GetAny (), port));

    ApplicationContainer sinkApp = sinkHelper.Install (nodes.Get (1));
    sinkApp.Start (Seconds (0.0));
    sinkApp.Stop (Seconds (simulationTime));

    // Create TCP source
    BulkSendHelper source (
        "ns3::TcpSocketFactory",
        sinkAddress);

    source.SetAttribute ("MaxBytes",
                         UintegerValue (0));

    source.SetAttribute ("SendSize",
                         UintegerValue (packetSize));

    ApplicationContainer sourceApp =
        source.Install (nodes.Get (0));

    sourceApp.Start (Seconds (1.0));
    sourceApp.Stop (Seconds (simulationTime));

    // Obtain TCP socket
    Ptr<Socket> ns3TcpSocket =
        Socket::CreateSocket (nodes.Get (0),
                              TcpSocketFactory::GetTypeId ());

    // Create cwnd trace file
    AsciiTraceHelper asciiTraceHelper;

    Ptr<OutputStreamWrapper> stream =
        asciiTraceHelper.CreateFileStream (
            "tcp-cwnd.tr");

    ns3TcpSocket->TraceConnectWithoutContext (
        "CongestionWindow",
        MakeBoundCallback (&CwndChange, stream));

    // Enable packet capture
    pointToPoint.EnablePcapAll ("tcp-congestion");

    // NetAnim
    AnimationInterface anim ("tcp-congestion.xml");

    // Run simulation
    Simulator::Stop (Seconds (simulationTime));

    Simulator::Run ();
    Simulator::Destroy ();

    return 0;
}
