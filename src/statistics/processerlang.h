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
#ifndef STATISTICSPROCESSERLANG_H
#define STATISTICSPROCESSERLANG_H

#include <statistics/process.h>

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

//@class Generates an Erlang distribution.
//@base public | Process.
class ProcessErlang : public Process
{
  //@cmember Erlang distribution parameter.
  double beta;
  //@cmember Stage number of Erlang distribution.
  int m;
public:
  //@cmember Creates a new Erlang distribution.
  ProcessErlang(){}
  //@cmember Creates a new Erlang distribution.
  ProcessErlang(double beta0,int m0);
  ProcessErlang(double beta0,int m0, int stream);
  //@cmember Return the number of stages.
  unsigned int GetStage();
  //@cmember Returns beta parameter.
  double GetBeta();
  //@cmember Sets the number of stages.
  int SetStage(int m0);
  //@cmember Sets beta parameter.
  int SetBeta(double beta0);
  //@cmember Returns a random number.
  virtual UDTIME DeltaProx();
};
}

#endif
