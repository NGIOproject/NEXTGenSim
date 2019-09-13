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
#ifndef STATISTICSPROCESSCONSTANT_H
#define STATISTICSPROCESSCONSTANT_H

#include <statistics/process.h>

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
//@class Generates a constant distribution.
//@base public | Process.
class ProcessConstant : public Process
{
  //@cmember Constant value.
  double k;
  public:
  //@cmember Creates a new constant distribution.
  ProcessConstant(){}
  //@cmember Creates a new constant distribution.
  ProcessConstant(double k0);
  //@cmember Returns the constant value.
  double GetConstant();
  //@cmember Sets the constant value.
  int SetConstant(double k0);
  //@cmember Returns a random number.
  virtual UDTIME DeltaProx();
};

}

#endif
