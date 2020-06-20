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
#include "Position_m.h"

#include "inet/applications/base/ApplicationPacket_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/TagBase_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/common/packet/printer/PacketPrinter.h"
#include "inet/networklayer/ipv4/Ipv4Header_m.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/physicallayer/contract/packetlevel/SignalTag_m.h"
#include "inet/networklayer/contract/ipv4/Ipv4Address.h"
#include "inet/physicallayer/common/packetlevel/Radio.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/physicallayer/base/packetlevel/TransmissionBase_m.h"



using namespace inet;

Define_Module(TDOAApp);

TDOAApp::~TDOAApp()
{
}

void TDOAApp::initialize(int stage)
{

    ApplicationBase::initialize(stage);


    if (stage == INITSTAGE_LOCAL) {
        EV << "Init" << endl;

        localPort = par("localPort");
        destPort = par("destPort");
        isReceiver = par("isReceiver");
        i=0;

//        processStart();
//        std::cout << "Socket created!" << endl;
//        sendPacket();
    }

}

void TDOAApp::setSocketOptions()
{
//    int timeToLive = par("timeToLive");
//    if (timeToLive != -1)
//        socket.setTimeToLive(timeToLive);
//
//    int dscp = par("dscp");
//    if (dscp != -1)
//        socket.setDscp(dscp);
//
//    int tos = par("tos");
//    if (tos != -1)
//        socket.setTos(tos);
//
//    const char *multicastInterface = par("multicastInterface");
//    if (multicastInterface[0]) {
//        IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
//        InterfaceEntry *ie = ift->findInterfaceByName(multicastInterface);
//        if (!ie)
//            throw cRuntimeError("Wrong multicastInterface setting: no interface named \"%s\"", multicastInterface);
//        socket.setMulticastOutputInterface(ie->getInterfaceId());
//    }
//
//    bool receiveBroadcast = par("receiveBroadcast");
//    if (receiveBroadcast)
//        socket.setBroadcast(true);
//
//    bool joinLocalMulticastGroups = par("joinLocalMulticastGroups");
//    if (joinLocalMulticastGroups) {
//        MulticastGroupList mgl = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this)->collectMulticastGroups();
//        socket.joinLocalMulticastGroups(mgl);
//    }
    socket.setCallback(this);
}

void TDOAApp::processStart()
{
    socket.setOutputGate(gate("socketOut"));


    const char *localAddress = par("localAddress");
    socket.bind(localPort);
    if(!isReceiver){
    socket.setBroadcast(false);
    }
    else{
        socket.setBroadcast(true);
    }

    setSocketOptions();

    const char *destAddrs = par("destAddresses");

    cStringTokenizer tokenizer(destAddrs);
    const char *token;


    while ((token = tokenizer.nextToken()) != nullptr) {
        if (strstr(token, "Broadcast") != nullptr){

            destAddresses.push_back(Ipv4Address::ALLONES_ADDRESS);

            }
        else {
            L3Address result;
            L3AddressResolver().tryResolve(token, result);
            if (result.isUnspecified())
                        EV_ERROR << "cannot resolve destination address: " << token << endl;

            destAddresses.push_back(result);
        }

    }

    if (destAddresses.empty()) {
        EV_ERROR << "Error sending - destAddresses empty" << endl;
    }

    if (isReceiver == false) {

        sendPacket();
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
    std::ostringstream packetType;

    if (!isReceiver) {

        packetType << "TDOA INIT";
    }
    else {
        packetType << "TDOA RESPONSE";
    }
    Packet *packet = new Packet((char*) packetType.str().c_str());

//    if(dontFragment)
//        packet->addTag<FragmentationReq>()->setDontFragment(true);
    const auto& payload = makeShared<ApplicationPacket>();

    payload->setChunkLength(B(4));
    payload->setSequenceNumber(1);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    packet->insertAtBack(payload);

    L3Address destAddr = chooseDestAddr();

    cModule *host = getContainingNode(this);

    std::cout << "host: " << host << endl;
    listener = new ReceptionStartedListener();
    host->subscribe("transmissionStarted", listener);

    emit(packetSentSignal, packet);



    socket.sendTo(packet, destAddr, destPort);



}

void TDOAApp::sendPacket(simtime_t startTime)
{
    std::ostringstream packetType;

    if (!isReceiver) {
        packetType << "TDOA INIT";
    }
    else {
        packetType << "TDOA RESPONSE";
    }
    Packet *packet = new Packet((char*) packetType.str().c_str());

//    if(dontFragment)
//        packet->addTag<FragmentationReq>()->setDontFragment(true);


    const auto& payload = makeShared<ApplicationPacket>();
    payload->setChunkLength(B(4));
    payload->setSequenceNumber(1);

    cModule *host = getContainingNode(this);
    IMobility *mobility = check_and_cast<IMobility *>(host->getSubmodule("mobility"));
    auto positions = mobility->getCurrentPosition();

    auto tags=payload->addTag<position>();
    tags->setLocation(positions);
    tags->setTime(startTime);
    packet->insertAtBack(payload);

    EV<<"Position: "<< positions;


    L3Address destAddr = chooseDestAddr();

    emit(packetSentSignal, packet);


    socket.sendTo(packet, destAddr, destPort);

}

void TDOAApp::handleMessageWhenUp(cMessage *msg)
{

    socket.processMessage(msg);

}

void TDOAApp::finish()
{

    for(i=0;i<3;i++){
        EV<< "Tempo: "<< valores[i].tempo<<endl;
        EV<< "Positon: "<< valores[i].position<<endl;
        EV<< "IP: "<< valores[i].ip<<endl;
    }
        EV<<"Tempo transmissão: "<<Tempo_Transmissao<<endl;
    ApplicationBase::finish();
}

void TDOAApp::socketDataArrived(UdpSocket *socket, Packet *packet)
{

    auto signalTimeTag = packet->getTag<SignalTimeInd>();
    auto startTime = signalTimeTag->getStartTime();
    EV << "startTime = " << startTime << endl;
    auto endTime = signalTimeTag->getEndTime();
    EV << "endTime = " << endTime << endl;
    cModule *host=getSystemModule()->getSubmodule("hostA");
    host->unsubscribe("transmissionStarted", listener);



    // process incoming packet
    emit(packetReceivedSignal, packet);


    if (isReceiver) {
        EV << "Receiver received packet: " << UdpSocket::getReceivedPacketInfo(packet) << endl;
        std::cout << "Receiver received packet: " << UdpSocket::getReceivedPacketInfo(packet) << endl;
//        delete packet;
       // Get the address from who sent the packet
        auto l3Addresses = packet->getTag<L3AddressInd>();
        L3Address srcAddr = l3Addresses->getSrcAddress();
        std::cout << "Source address: " << srcAddr << endl;
        destAddresses.push_back(srcAddr);
        delete packet;

        sendPacket(startTime);

    }
    else {
        EV << "Sender received packet: " << UdpSocket::getReceivedPacketInfo(packet) << endl;

        // Extract time response to save.

        auto data = packet->peekData(); // get all data from the packet
        std::cout << data << endl;
        auto regions = data->getAllTags<position>(); // get all tag regions
        auto l3Addresses = packet->getTag<L3AddressInd>();
        L3Address srcAddr = l3Addresses->getSrcAddress();


        for (auto& region : regions) { // for each region do

            auto creationTime = region.getTag()->getTime(); // original time
            auto positions=region.getTag()->getLocation();
            EV << "timeReceiver = " << creationTime << "Position :"<<positions<< endl;
            valores[i].tempo=creationTime;
            valores[i].position=positions;
            valores[i].ip=srcAddr;
        }

        i++;


        delete packet;
    }
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
    processStart();
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

void TDOAApp::setTimeSent(simtime_t time)
{
    Tempo_Transmissao=time;


}

