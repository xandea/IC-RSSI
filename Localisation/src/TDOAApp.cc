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

#include "TDOAApp.h"

#include "inet/applications/base/ApplicationPacket_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/TagBase_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"

using namespace inet;

Define_Module(TDOAApp);

TDOAApp::~TDOAApp()
{
}

void TDOAApp::initialize()
{
    localPort = par("localPort");
    destPort = par("destPort");
    createSocket();
    sendPacket();
}

void TDOAApp::createSocket()
{
    socket.setOutputGate(gate("socketOut"));
    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);

    const char *destAddrs = par("destAddresses");
    cStringTokenizer tokenizer(destAddrs);
    const char *token;

    while ((token = tokenizer.nextToken()) != nullptr) {
        destAddressStr.push_back(token);
        L3Address result;
        L3AddressResolver().tryResolve(token, result);
        if (result.isUnspecified())
            EV_ERROR << "cannot resolve destination address: " << token << endl;
        destAddresses.push_back(result);
    }

    if (destAddresses.empty()) {
        EV_ERROR << "Error sending - destAddresses empty" << endl;
    }
}

L3Address TDOAApp::chooseDestAddr()
{
    int k = intrand(destAddresses.size());
    if (destAddresses[k].isUnspecified() || destAddresses[k].isLinkLocal()) {
        L3AddressResolver().tryResolve(destAddressStr[k].c_str(), destAddresses[k]);
    }
    return destAddresses[k];
}

void TDOAApp::sendPacket()
{
    Packet *packet = new Packet("test");
//    if(dontFragment)
//        packet->addTag<FragmentationReq>()->setDontFragment(true);
    const auto& payload = makeShared<ApplicationPacket>();
    payload->setChunkLength(B(par("messageLength")));
    payload->setSequenceNumber(1);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    packet->insertAtBack(payload);
    L3Address destAddr = chooseDestAddr();
    socket.sendTo(packet, destAddr, destPort);

}

void TDOAApp::handleMessageWhenUp(cMessage *msg)
{

    std::cout << "handleMessage" << msg;
}

void TDOAApp::finish()
{
    ApplicationBase::finish();
}

void TDOAApp::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    // process incoming packet
    std::cout << "data arrived " << endl;
}

void TDOAApp::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void TDOAApp::socketClosed(UdpSocket *socket)
{
}


void TDOAApp::handleStartOperation(LifecycleOperation *operation)
{
}

void TDOAApp::handleStopOperation(LifecycleOperation *operation)
{
    socket.close();
}

void TDOAApp::handleCrashOperation(LifecycleOperation *operation)
{
    socket.destroy();
}

void TDOAApp::refreshDisplay() const
{
    ApplicationBase::refreshDisplay();

//    char buf[100];
//    sprintf(buf, "rcvd: %d pks\nsent: %d pks", numReceived, numSent);
//    getDisplayString().setTagArg("t", 0, buf);
}
