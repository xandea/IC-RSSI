

@license(LGPL);

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.networklayer.ipv4.IIpv4;
import inet.node.inet.INetworkNode;
import inet.visualizer.contract.IIntegratedVisualizer;
import inet.physicallayer.contract.packetlevel.IRadioMedium;

network WirelessA
{
    parameters:
        @display("bgb=666.288,531.216;bgg=100,1,grey95");
        @figure[title](type=label; pos=0,-1; anchor=sw; color=darkblue);

        @figure[rcvdPkText](type=indicatorText; pos=380,20; anchor=w; font=,18; textFormat="packets received: %g"; initialValue=0);
        @statistic[packetReceived](source=hostB.app[0].packetReceived; record=figure(count); targetFigure=rcvdPkText);

    submodules:
        visualizer: <default("IntegratedCanvasVisualizer")> like IIntegratedVisualizer if hasVisualizer() {
            @display("p=580,125");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=580,200");
        }
        radioMedium: <default("UnitDiskRadioMedium")> like IRadioMedium {
            @display("p=580,275");
        }
        hostA: <default("WirelessHost")> like INetworkNode {
            @display("p=286.00,333.00");
        }
        hostB: <default("WirelessHost")> like INetworkNode {
            @display("p=495.00,340.00");
        }
        hostR1: <default("WirelessHost")> like INetworkNode {
            @display("p=110.00,210.00");
        }
        hostR2: <default("WirelessHost")> like INetworkNode {
            @display("p=270.00,498.00");
        }

}

