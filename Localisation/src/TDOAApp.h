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

#include "inet/common/INETDefs.h"
#include "Valores.h"
#include "ReceptionStartedListener.h"

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
    int localPort = -1, destPort = -1;
    bool isReceiver;
    UdpSocket socket;
    int i;
    Valores valores[3];
    simsignal_t transmissionStarted;
    ReceptionStartedListener *listener;

  protected:
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void finish() override;
    virtual void refreshDisplay() const override;


    virtual L3Address chooseDestAddr();
    virtual void sendPacket();
    virtual void sendPacket(simtime_t startTime);
    virtual void processStart();
    virtual void setSocketOptions();

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;

  public:
    TDOAApp() {}
    ~TDOAApp();
    void setTimeSent(simtime_t timeSent);
};

#endif
