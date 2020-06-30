/*
 * Valores.h
 *
 *  Created on: Jun 16, 2020
 *      Author: angel
 */

#ifndef VALORES_H_
#define VALORES_H_

#include "inet/common/INETDefs.h"
#include "inet/common/geometry/common/Coord.h"
#include "inet/networklayer/common/L3Address.h"

using namespace inet;

class Valores
{
    public:
    simtime_t tempo;
    simtime_t timeSent;
    Coord position;
    L3Address ip;
    double distancia;
    std::chrono::duration<double> time_real_distancia ;
};



#endif /* VALORES_H_ */
