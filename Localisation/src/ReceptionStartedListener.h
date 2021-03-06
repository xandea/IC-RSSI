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

#ifndef RECEPTIONSTARTEDLISTENER_H_
#define RECEPTIONSTARTEDLISTENER_H_
#include "inet/common/INETDefs.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/Simsignals.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/applications/base/ApplicationBase.h"



using namespace inet;

class INET_API ReceptionStartedListener : public cListener, public cSimpleModule
{
protected:
    virtual void receiveSignal(cComponent* source, simsignal_t signalID, cObject* obj, cObject* details) override;
    UdpSocket socket;

public:
    ReceptionStartedListener();
    virtual ~ReceptionStartedListener();
};

#endif /* RECEPTIONSTARTEDLISTENER_H_ */
