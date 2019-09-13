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
#ifndef STATISTICSPROCESSGEOMETRIC_H
#define STATISTICSPROCESSGEOMETRIC_H

#include <statistics/process.h>

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

//@class Generates a Geometric distribution.
//@base public | Process.
class ProcessGeometric : public Process
{
  //@cmember Probability of succes in each prove.
  double p;
public:
  //@cmember Creates a new geometric distribution.
  ProcessGeometric(){}
  //@cmember Creates a new geometric distribution.
  ProcessGeometric(double p0, int stream);
  ProcessGeometric(double p0);

  //@cmember Returns succes probability.
  double GetProbability();
  //@cmember Sets succes probability.
  int SetProbability(double p0);
  //@cmember Returns a random number.
  virtual UDTIME DeltaProx();
};

}

#endif
