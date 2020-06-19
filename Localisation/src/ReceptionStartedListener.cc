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

#include "ReceptionStartedListener.h"


#include "inet/common/ModuleAccess.h"
#include "inet/common/Simsignals.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

using namespace inet;

ReceptionStartedListener::ReceptionStartedListener() {
}

ReceptionStartedListener::~ReceptionStartedListener()
{

}

void ReceptionStartedListener::receiveSignal(cComponent* source, simsignal_t signalID, cObject* obj, cObject* details)
{
    EV << "Listener tempo: "<< simTime();
    cModule *targetModule = getSystemModule()->getSubmodule("hostA",0);//->getModuleType();
    cMessage *teste= new cMessage();
    teste->setTimestamp(simTime());
    cMessage* copy = check_and_cast<cMessage*>(teste->dup());



    Enter_Method_Silent("receiveSignal");

    sendDirect(copy, targetModule->getSubmodule("app",0),targetModule->getSubmodule("app",0)->gate("in")->getId());


}




