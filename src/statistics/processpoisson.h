/*
* Copyright 2007 Francesc Guim Bernat & Barcelona Supercomputing Centre (fguim@pcmas.ac.upc.edu)
* Copyright 2019 Daniel Rivas & Barcelona Supercomputing Centre (daniel.rivas@bsc.es)
* Copyright 2015-2019 NEXTGenIO Project [EC H2020 Project ID: 671951] (www.nextgenio.eu)
*
* This file is part of NEXTGenSim.
*
* NEXTGenSim is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* NEXTGenSim is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with NEXTGenSim.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef STATISTICSPROCESSPOISSON_H
#define STATISTICSPROCESSPOISSON_H

#include <statistics/process.h>

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
//@class Generates a Poission distribution.
//@base public | Process.
class ProcessPoisson : public Process
{
  //@cmember Frequency of entities arrivals.
  double lambda;
public:
  //@cmember Creates a new poission distribution.
  ProcessPoisson(){}
  //@cmember Creates a new poission distribution.
  ProcessPoisson(double lambda0, int stream);
  ProcessPoisson(double lambda0);
  //@cmember Returns the frequency of enities arrivals.
  double GetMu();
  //@cmember Sets the frequency of enities arrivals.
  int SetMu(double lambda0);
  //@cmember Returns a random number.
  virtual UDTIME DeltaProx();
};

}

#endif
