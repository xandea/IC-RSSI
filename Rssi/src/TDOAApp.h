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

#ifndef __TDOA_TDOAAPP_H_
#define __TDOA_TDOAAPP_H_

#include <vector>
#include <Eigen>
#include <chrono>

#include "inet/common/INETDefs.h"
#include "Valores.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/networklayer/contract/ipv4/Ipv4Address.h"
#include "inet/common/INETDefs.h"
#include "inet/physicallayer/common/packetlevel/Radio.h"
#include "inet/physicallayer/contract/packetlevel/IRadio.h"


using namespace inet;

/**
 * TODO - Generated class
 */
class INET_API TDOAApp : public ApplicationBase, public UdpSocket::ICallback
{
    protected:

    std::vector<L3Address> destAddresses;
    std::vector<std::string> destAddressStr;
    std::chrono::time_point<std::chrono::system_clock> start_trilateracao, end_trilateracao;
    std::chrono::time_point<std::chrono::system_clock> start_distancia, end_distancia;
    int localPort = -1, destPort = -1;
    bool isReceiver;
    UdpSocket socket;
    Coord position_emissor;
    int i;
    int Quant_nos=0;
    double x_emissor, y_emissor;
    std::vector <Valores> valores;
    const char* nome_arquivo;








  protected:
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void finish() override;
    virtual void refreshDisplay() const override;


    virtual L3Address chooseDestAddr();
    virtual void sendPacket();
    virtual void sendPacket(W signalPower);
    virtual void processStart();
    virtual void setSocketOptions();
    virtual double calculo(double x, double y, double z,W potencia);
    virtual void trilateracao();

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;

  public:
    TDOAApp() {}
    ~TDOAApp();
};

#endif
