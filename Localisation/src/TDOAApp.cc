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
#include <chrono>
#include <Eigen>

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
#include "inet/common/scheduler/RealTimeScheduler.h"
#include <fstream>



using namespace inet;

Define_Module(TDOAApp);

TDOAApp::~TDOAApp()
{
}

void TDOAApp::initialize(int stage)
{


    ApplicationBase::initialize(stage);
    Velocidade_luz=299792458;


    if (stage == INITSTAGE_LOCAL) {
        EV << "Init" << endl;

        localPort = par("localPort");
        destPort = par("destPort");
        isReceiver = par("isReceiver");
        nome_arquivo=par("Nome_arquivo");
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
    if (!isReceiver)
    {
        EV<<"Tempo transmissão: "<<Tempo_Transmissao<<endl;

        start_trilateracao = std::chrono::system_clock::now();
        Trilateracao();
        end_trilateracao = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = end_trilateracao - start_trilateracao;

        EV<<"Tempo trilateracao "<<elapsed_seconds.count()<<endl;


        std::ifstream verifica_arquivo;
        std::ofstream arquivo;

        verifica_arquivo.open(nome_arquivo);

        if(verifica_arquivo.is_open()){
            arquivo.open (nome_arquivo,std::ios::app);
            arquivo<<elapsed_seconds.count()<<";";
            int Quant_nos=valores.size();
            for(int cont=0; cont<Quant_nos;cont++){
                arquivo<<valores.at(cont).time_real_distancia.count()<<";";
            }

            for(int cont=0; cont<Quant_nos;cont++){
                  arquivo<<valores.at(cont).distancia<<";";
             }
            arquivo<<x_emissor<<";"<<y_emissor<<";";
            arquivo<<position_emissor.x<<";"<<position_emissor.y;

            arquivo<<"\n";
        }


        else{
            arquivo.open (nome_arquivo);
            arquivo << "Tempo Trilateração (s);";

            int Quant_nos=valores.size();

            for(int cont=0; cont<Quant_nos;cont++){

                EV<<"Tempo distancia :"<<valores.at(cont).time_real_distancia.count()<<endl;

                arquivo<<"Tempo distancia "<< cont<<"(s);";
            }
            for(int cont=0; cont<Quant_nos;cont++){
                arquivo<<"Distancia nó "<<cont<<"(m);";
            }
            arquivo<<"Posição x estimada (m);";
            arquivo<<"Posição y estimada (m);";
            arquivo<<"Posição x real (m);";
            arquivo<<"Posição y real (m)";


            arquivo<<"\n";
            arquivo<<elapsed_seconds.count()<<";";

            for(int cont=0; cont<Quant_nos;cont++){
                arquivo<<valores.at(cont).time_real_distancia.count()<<";";
            }

            for(int cont=0; cont<Quant_nos;cont++){
                  arquivo<<valores.at(cont).distancia<<";";
             }
            arquivo<<x_emissor<<";"<<y_emissor<<";";
            arquivo<<position_emissor.x<<";"<<position_emissor.y;
            arquivo<<"\n";
        }
        arquivo.close();
        verifica_arquivo.close();
 }

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
        cModule *host = getContainingNode(this);
        IMobility *mobility = check_and_cast<IMobility *>(host->getSubmodule("mobility"));
        auto positions = mobility->getCurrentPosition();

        position_emissor=positions;

        for (auto& region : regions) { // for each region do

            auto creationTime = region.getTag()->getTime(); // original time
            auto positions=region.getTag()->getLocation();
            EV << "timeReceiver = " << creationTime << "Position :"<<positions<< endl;

            start_distancia = std::chrono::system_clock::now();

            auto distancia=CalculoDistancia(Tempo_Transmissao,creationTime);

            end_distancia = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end_distancia - start_distancia;


            Valores valores_temp;
            valores_temp.time_real_distancia=elapsed_seconds;
            valores_temp.distancia=distancia;
            valores_temp.position=positions;
            valores_temp.tempo=creationTime;
            valores_temp.ip=srcAddr;

            valores.push_back(valores_temp);
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
    // Add random delay
    Tempo_Transmissao = Tempo_Transmissao+uniform(0, 0.000000083);

}

double TDOAApp::CalculoDistancia(simtime_t tempo_inicial, simtime_t tempo_final){
    simtime_t diferenca_tempo= tempo_final - tempo_inicial;

    double di=diferenca_tempo.dbl()* Velocidade_luz;

    EV<<"Distancia :"<< di;
    return di;

}

void TDOAApp::Trilateracao(){

    int Quant_nos=valores.size();
    double x[Quant_nos];
    double y[Quant_nos];
    double d[Quant_nos];


    EV<<"Quantidade de nos: "<< Quant_nos<<endl;

    for(int cont=0; cont<Quant_nos;cont++){
        x[cont]=valores.at(cont).position.x;
    }
    for(int cont=0; cont<Quant_nos;cont++){
        y[cont]=valores.at(cont).position.y;
    }
    for(int cont=0; cont<Quant_nos;cont++){
        d[cont]=valores.at(cont).distancia;
     }


    Eigen::MatrixXd matriz_X(Quant_nos-1,2);
    Eigen::MatrixXd matriz_Y(Quant_nos-1,1);

    int posicao;

    for(posicao=0;posicao<Quant_nos-1;posicao++){
        matriz_X(posicao,0)=2*(x[Quant_nos-1]-x[posicao]);
    }
    for(posicao=0; posicao<Quant_nos-1;posicao++){
        matriz_X(posicao,1)=2*(y[Quant_nos-1]-y[posicao]);
    }
    for(posicao=0; posicao<Quant_nos-1;posicao++){
        matriz_Y(posicao,0)=(-(pow(x[posicao],2))-(pow(y[posicao],2))+(pow(d[posicao],2))) - (-(pow(x[Quant_nos-1],2))-(pow(y[Quant_nos-1],2))+pow(d[Quant_nos-1],2));
    }


    Eigen::MatrixXd matriz_X_inversa(2,2);

    EV<<"Matriz_X: "<<endl<< matriz_X<<endl;
    EV<<"Matriz_Y: "<<endl<< matriz_Y<<endl;
    matriz_X_inversa=(matriz_X.transpose()*matriz_X).inverse();
    matriz_X=matriz_X_inversa*matriz_X.transpose()*matriz_Y;
    EV<<"Resultado: "<<endl<< matriz_X<<endl;

    x_emissor=matriz_X(0,0);
    y_emissor=matriz_X(1,0);



}
