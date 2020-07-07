//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 


import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.INetworkNode;
import inet.physicallayer.contract.packetlevel.IRadioMedium;
import inet.visualizer.contract.IIntegratedVisualizer;

network Wireless_12_10
{
    parameters:
        @display("bgb=650,500;bgg=100,1,grey95");
        @figure[title](type=label; pos=0,-1; anchor=sw; color=darkblue);

        @figure[rcvdPkText](type=indicatorText; pos=380,20; anchor=w; font=,18; textFormat="packets received: %g"; initialValue=0);
//        @statistic[packetReceived](source=hostB.app[0].packetReceived; record=figure(count); targetFigure=rcvdPkText);

    submodules:
        visualizer: <default("IntegratedCanvasVisualizer")> like IIntegratedVisualizer if hasVisualizer() {
            @display("p=477.79202,144.144");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=477.79202,218.73601");
        }
        radioMedium: <default("UnitDiskRadioMedium")> like IRadioMedium {
            @display("p=477.79202,299.376");
        }
         hostA: <default("WirelessHost")> like INetworkNode {
            @display("p=300,300");
        }
        hostB: <default("WirelessHost")> like INetworkNode {
            @display("p=305.0,308.660254038");
        }
        hostC: <default("WirelessHost")> like INetworkNode {
            @display("p=295.0,308.660254038");
        }
        hostD: <default("WirelessHost")> like INetworkNode {
            @display("p=290.0,300");
        }
        hostE: <default("WirelessHost")> like INetworkNode {
            @display("p=295.0,291.339745962");
        }
        hostF: <default("WirelessHost")> like INetworkNode {
            @display("p=310.0,300.0");
        }
        hostG: <default("WirelessHost")> like INetworkNode {
            @display("p=305.0,291.339745962");
        }
        hostH: <default("WirelessHost")> like INetworkNode {
            @display("p=308.660254038,305.0");
        }
        hostI: <default("WirelessHost")> like INetworkNode {
            @display("p=308.660254038,295.0");
        }
        hostJ: <default("WirelessHost")> like INetworkNode {
            @display("p=291.339745962,295.0");
        }
        hostK: <default("WirelessHost")> like INetworkNode {
            @display("p=291.339745962,305.0");
        }
        hostL: <default("WirelessHost")> like INetworkNode {
            @display("p=300.0,310.0");
        }
        hostM: <default("WirelessHost")> like INetworkNode {
            @display("p=300.0,290.0");
        }

}
